``` bash

(gdb) disas main
Dump of assembler code for function main:
   0x0804847c <+0>:	    push   ebp
   0x0804847d <+1>:	    mov    ebp,esp
   0x0804847f <+3>:	    and    esp,0xfffffff0
   0x08048482 <+6>:	    sub    esp,0x20
   0x08048485 <+9>:	    mov    DWORD PTR [esp],0x40                     # dest
   0x0804848c <+16>:	call   0x8048350 <malloc@plt>
   0x08048491 <+21>:	mov    DWORD PTR [esp+0x1c],eax                 # adresse de dest = esp+0x1c
   0x08048495 <+25>:	mov    DWORD PTR [esp],0x4                      # var
   0x0804849c <+32>:	call   0x8048350 <malloc@plt>
   0x080484a1 <+37>:	mov    DWORD PTR [esp+0x18],eax                 # adresse de var a esp+0x18
   0x080484a5 <+41>:	mov    edx,0x8048468                            # REGARDER la valeur == m
   0x080484aa <+46>:	mov    eax,DWORD PTR [esp+0x18]                 # on met adresse de var dans eax
   0x080484ae <+50>:	mov    DWORD PTR [eax],edx                         
   0x080484b0 <+52>:	mov    eax,DWORD PTR [ebp+0xc]
   0x080484b3 <+55>:	add    eax,0x4
   0x080484b6 <+58>:	mov    eax,DWORD PTR [eax]
   0x080484b8 <+60>:	mov    edx,eax
   0x080484ba <+62>:	mov    eax,DWORD PTR [esp+0x1c]
   0x080484be <+66>:	mov    DWORD PTR [esp+0x4],edx
   0x080484c2 <+70>:	mov    DWORD PTR [esp],eax
   0x080484c5 <+73>:	call   0x8048340 <strcpy@plt>
   0x080484ca <+78>:	mov    eax,DWORD PTR [esp+0x18]
   0x080484ce <+82>:	mov    eax,DWORD PTR [eax]
   0x080484d0 <+84>:	call   eax
   0x080484d2 <+86>:	leave  
   0x080484d3 <+87>:	ret    
End of assembler dump.


(gdb) b *main+46
Breakpoint 2 at 0x80484aa

(gdb) x/10x $edx
0x8048468 <m>:	0x55	0x89	0xe5	0x83	0xec	0x18	0xc7	0x04

(gdb) b *main+78
Breakpoint 1 at 0x80484ca
(gdb) run aaaaaaaaaaaaaa
Starting program: /home/user/level6/level6 aaaaaaaaaaaaaa

(gdb) x/14x $eax
0x804a008:	0x61	0x61	0x61	0x61	0x61	0x61	0x61	0x61
0x804a010:	0x61	0x61	0x61	0x61	0x61	0x61

(gdb) x/14x $esp+0x1c
0xbffff71c:	0x08	0xa0	0x04	0x08	0xe0	0x84	0x04	0x08
0xbffff724:	0x00	0x00	0x00	0x00	0x00	0x00

```