# LEVEL0

Le systeme de fichier se présente de cette manière:

```Diff
level0@RainFall:~$ ls -la
total 737
dr-xr-x---+ 1 level0 level0     60 Mar  6  2016 .
dr-x--x--x  1 root   root      340 Sep 23  2015 ..
rw-r--r--  1 level0 level0    220 Apr  3  2012 .bash_logout
rw-r--r--  1 level0 level0   3530 Sep 23  2015 .bashrc
+-rwsr-x---+ 1 level1 users  747441 Mar  6  2016 level0
rw-r--r--  1 level0 level0    675 Apr  3  2012 .profile
```

Le fichier `level0` est un exécutable.

Le bit `SUID` est activé, l'utilisateur `level0` peut exécuter le programme avec les droits de `level1`.

Le code assembleur se présente comme ci-dessous:

```Diff
   0x08048ec0 <+0>:	push   ebp
   0x08048ec1 <+1>:	mov    ebp,esp
   0x08048ec3 <+3>:	and    esp,0xfffffff0
   0x08048ec6 <+6>:	sub    esp,0x20
   0x08048ec9 <+9>:	mov    eax,DWORD PTR [ebp+0xc]
   0x08048ecc <+12>:	add    eax,0x4
   0x08048ecf <+15>:	mov    eax,DWORD PTR [eax]
   0x08048ed1 <+17>:	mov    DWORD PTR [esp],eax
+  0x08048ed4 <+20>:	call   0x8049710 <atoi>
+  0x08048ed9 <+25>:	cmp    eax,0x1a7
+  0x08048ede <+30>:	jne    0x8048f58 <main+152>
   0x08048ee0 <+32>:	mov    DWORD PTR [esp],0x80c5348
...
+  0x08048f01 <+65>:	call   0x8054670 <geteuid>
...
+  0x08048f3d <+125>:	call   0x8054690 <setresuid>
...
   0x08048f4a <+138>:	mov    DWORD PTR [esp],0x80c5348
+  0x08048f51 <+145>:	call   0x8054640 <execv>
   0x08048f56 <+150>:	jmp    0x8048f80 <main+192>
   0x08048f58 <+152>:	mov    eax,ds:0x80ee170
   0x08048f5d <+157>:	mov    edx,eax
   0x08048f5f <+159>:	mov    eax,0x80c5350
   0x08048f64 <+164>:	mov    DWORD PTR [esp+0xc],edx
   0x08048f68 <+168>:	mov    DWORD PTR [esp+0x8],0x5
   0x08048f70 <+176>:	mov    DWORD PTR [esp+0x4],0x1
   0x08048f78 <+184>:	mov    DWORD PTR [esp],eax
   0x08048f7b <+187>:	call   0x804a230 <fwrite>
   0x08048f80 <+192>:	mov    eax,0x0
   0x08048f85 <+197>:	leave  
   0x08048f86 <+198>:	ret
```

On y voit:

- `<main+20>`: un appel à la fonction `atoi`
- `<main+25>`: une comparaison entre le retour de `atoi` stocké dans `eax` et `0x1a7`

Si `eax` est diférent de `0x1a7` le programme saute directement vers `<main+152>`.

Sinon le programe récupère `EUID` avec la fonction `geteuid` (`<main+65>`), le set avec `setresuid` (`<main+125>`) et lance la fonction `execve` (`<main+145>`).

Via gdb on peut obtenir le programme qui sera lancé par `execve`:

```Diff
(gdb) x/s *(char **)$esp
+0x80c5348:	 "/bin/sh"
```

C'est donc un shell qui sera exécuter avec les droits de `level1` (ça permet de passer les protections d'`execve`). On va donc pouvoir `cat` le fichier `.pass` présent dans le `home` du user `level1`.

Il nous faut donc éviter le jump après la fonction `atoi` afin d'atteindre `execve`. Pour celà on doit trouver la valeur exacte de `0x1a7`:

```Diff
(gdb) b atoi
Breakpoint 1 at 0x8049710
(gdb) run 25
Starting program: /home/user/level0/level0 25

Breakpoint 1, 0x08049710 in atoi ()
(gdb) finish
Run till exit from #0  0x08049710 in atoi ()
0x08048ed9 in main ()
+(gdb) p/d 0x1a7
+$1 = 423
(gdb) 
```

`0x1a7` = 423 => On relance le programme avec cette valeur en argument:

```Diff
(gdb) run 423
...
...
+process 3612 is executing new program: /bin/dash
Error in re-setting breakpoint 1: Function "atoi" not defined.
+$ #on est dans le shell
```

Une fois le shell lancé il ne reste plus qu'à `cat` le fichier `.pass`.

```
level0@RainFall:~$ ./level0 423
$ cat /home/user/level1/.pass
1fe8a524fa4bec01ca4ea2a869af2a02260d4a7d5fe7e7c24d8617e6dca12d3a
$ 
```