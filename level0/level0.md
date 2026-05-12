# LEVEL0

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


```Diff
0x08048ec0 <+0>:	push   %ebp
   0x08048ec1 <+1>:	mov    %esp,%ebp
   0x08048ec3 <+3>:	and    $0xfffffff0,%esp
   0x08048ec6 <+6>:	sub    $0x20,%esp
   0x08048ec9 <+9>:	mov    0xc(%ebp),%eax
   0x08048ecc <+12>:	add    $0x4,%eax
   0x08048ecf <+15>:	mov    (%eax),%eax
   0x08048ed1 <+17>:	mov    %eax,(%esp)
+   0x08048ed4 <+20>:	call   0x8049710 <atoi>
+   0x08048ed9 <+25>:	cmp    $0x1a7,%eax
+   0x08048ede <+30>:	jne    0x8048f58 <main+152>
   0x08048ee0 <+32>:	movl   $0x80c5348,(%esp)
...
...
...
   0x08048f4a <+138>:	movl   $0x80c5348,(%esp)
+   0x08048f51 <+145>:	call   0x8054640 <execv>
   0x08048f56 <+150>:	jmp    0x8048f80 <main+192>
+   0x08048f58 <+152>:	mov    0x80ee170,%eax
   0x08048f5d <+157>:	mov    %eax,%edx
   0x08048f5f <+159>:	mov    $0x80c5350,%eax
   0x08048f64 <+164>:	mov    %edx,0xc(%esp)
   0x08048f68 <+168>:	movl   $0x5,0x8(%esp)
   0x08048f70 <+176>:	movl   $0x1,0x4(%esp)
   0x08048f78 <+184>:	mov    %eax,(%esp)
   0x08048f7b <+187>:	call   0x804a230 <fwrite>
   0x08048f80 <+192>:	mov    $0x0,%eax
   0x08048f85 <+197>:	leave  
   0x08048f86 <+198>:	ret
```

```
(gdb) b atoi
Breakpoint 1 at 0x8049710
(gdb) run 25
Starting program: /home/user/level0/level0 25

Breakpoint 1, 0x08049710 in atoi ()
(gdb) finish
Run till exit from #0  0x08049710 in atoi ()
0x08048ed9 in main ()
(gdb) p/d 0x1a7
$1 = 423
(gdb) 
```

```
(gdb) run 423
The program being debugged has been started already.
Start it from the beginning? (y or n) y
Starting program: /home/user/level0/level0 423

Breakpoint 1, 0x08049710 in atoi ()
(gdb) finish
Run till exit from #0  0x08049710 in atoi ()
0x08048ed9 in main ()
(gdb) n
Single stepping until exit from function main,
which has no line number information.
process 3612 is executing new program: /bin/dash
Error in re-setting breakpoint 1: Function "atoi" not defined.
$ 
```

```
level0@RainFall:~$ ./level0 423
$ cat /home/user/level1/.pass
1fe8a524fa4bec01ca4ea2a869af2a02260d4a7d5fe7e7c24d8617e6dca12d3a
$ 
```