## ASM level7

## Allocations

``` bash
(gdb) disas main
Dump of assembler code for function main:
   [...]
   0x0804852a <+9> :	mov    DWORD PTR [esp],0x8 
   0x08048531 <+16>:	call   0x80483f0 <malloc@plt>               # malloc(8) --> 0x804a008
   0x08048536 <+21>:	mov    DWORD PTR [esp+0x1c],eax             # sauvegarde adresse data1
   0x0804853a <+25>:	mov    eax,DWORD PTR [esp+0x1c]
   0x0804853e <+29>:	mov    DWORD PTR [eax],0x1                  # data1[0] = 1

   0x08048544 <+35>:	mov    DWORD PTR [esp],0x8                  
   0x0804854b <+42>:	call   0x80483f0 <malloc@plt>               # malloc(8) -->  0x804a018
   0x08048550 <+47>:	mov    edx,eax                              # edx = 0x804a018
   0x08048552 <+49>:	mov    eax,DWORD PTR [esp+0x1c]             # eax = adresse data1 (0x804a008)
   0x08048556 <+53>:	mov    DWORD PTR [eax+0x4],edx              # data1[1] = 0x804a018 (ptr vers buffer1)

   0x08048559 <+56>:	mov    DWORD PTR [esp],0x8                  
   0x08048560 <+63>:	call   0x80483f0 <malloc@plt>               # malloc(8) --> 0x804a028
   0x08048565 <+68>:	mov    DWORD PTR [esp+0x18],eax             # sauvegarde adresse data2
   0x08048569 <+72>:	mov    eax,DWORD PTR [esp+0x18]
   0x0804856d <+76>:	mov    DWORD PTR [eax],0x2                  # data2[0] = 2
   0x08048573 <+82>:	mov    DWORD PTR [esp],0x8

   0x0804857a <+89>:	call   0x80483f0 <malloc@plt>               # malloc(8) --> 0x804a038
    0x0804857f <+94>:	mov    edx,eax                              # edx = 0x0804a038 
    0x08048581 <+96>:	mov    eax,DWORD PTR [esp+0x18]             # eax = adresse data2 (0x0804a028)
    0x08048585 <+100>:	mov    DWORD PTR [eax+0x4],edx              # data2[1] = 0x804a038 (ptr vers buffer2)
  [...]
   0x08048601 <+224>:	leave  
   0x08048602 <+225>:	ret    
End of assembler dump.
```

## strcpy

``` bash
(gdb) disas main
Dump of assembler code for function main:
    [...]
    0x08048588 <+103>:	mov    eax,DWORD PTR [ebp+0xc]      # argv
    0x0804858b <+106>:	add    eax,0x4                      # argv[1]
    0x0804858e <+109>:	mov    eax,DWORD PTR [eax]
    0x08048590 <+111>:	mov    edx,eax
    0x08048592 <+113>:	mov    eax,DWORD PTR [esp+0x1c]     # adresse data1 (0x804a008)
    0x08048596 <+117>:	mov    eax,DWORD PTR [eax+0x4]      # eax = data1[1] = 0x0804a018 (buffer1)
    0x08048599 <+120>:	mov    DWORD PTR [esp+0x4],edx      # 2e arg = argv[1]
    0x0804859d <+124>:	mov    DWORD PTR [esp],eax          # 1er arg = buffer1 (== data1[1])
    0x080485a0 <+127>:	call   0x80483e0 <strcpy@plt>       # strcpy((char*)data1[1], argv[1])

    0x080485a5 <+132>:	mov    eax,DWORD PTR [ebp+0xc]      # argv
    0x080485a8 <+135>:	add    eax,0x8                      # argv[2]
    0x080485ab <+138>:	mov    eax,DWORD PTR [eax]
    0x080485ad <+140>:	mov    edx,eax
    0x080485af <+142>:	mov    eax,DWORD PTR [esp+0x18]     # eax = adresse data2 (0x0804a028)
    0x080485b3 <+146>:	mov    eax,DWORD PTR [eax+0x4]      # eax = data2[1] = 0x0804a038 (buffer2)
    0x080485b6 <+149>:	mov    DWORD PTR [esp+0x4],edx      # 2e arg = argv[2]
    0x080485ba <+153>:	mov    DWORD PTR [esp],eax          # 1er arg = buffer2 (== data2[1])
    0x080485bd <+156>:	call   0x80483e0 <strcpy@plt>       # strcpy((char*)data2[1], argv[2])
    [...]
```

## Lecture du mot de passe

``` bash
(gdb) disas main
    [...]
    0x080485c2 <+161>:	mov    edx,0x80486e9                # "r" "/home/user/level8/.pass" "~~"
    0x080485c7 <+166>:	mov    eax,0x80486eb                # "/home/user/level8/.pass"
    0x080485cc <+171>:	mov    DWORD PTR [esp+0x4],edx      # 2e arg = "r"
    0x080485d0 <+175>:	mov    DWORD PTR [esp],eax          # 1e arg = "/home/user/level8/.pass"
    0x080485d3 <+178>:	call   0x8048430 <fopen@plt>        # fopen("/home/user/level8/.pass", "r");
    0x080485d8 <+183>:	mov    DWORD PTR [esp+0x8],eax      # 3eme arg (file == eax)
    0x080485dc <+187>:	mov    DWORD PTR [esp+0x4],0x44     # 2eme arg (68 == taille max)
    0x080485e4 <+195>:	mov    DWORD PTR [esp],0x8049960    # 1e arg buffer global (0x8049960 == c) 
    0x080485eb <+202>:	call   0x80483c0 <fgets@plt>        # fgets(c, 68, file) == recupère le mdp
    0x080485f0 <+207>:	mov    DWORD PTR [esp],0x8048703    # "~~"
    0x080485f7 <+214>:	call   0x8048400 <puts@plt>         # puts("~~")
    [...]
```

``` bash
1: x/i $pc
=> 0x80485cc <main+171>:	mov    DWORD PTR [esp+0x4],edx
(gdb) x/4s $edx
0x80486e9:	 "r"
0x80486eb:	 "/home/user/level8/.pass"
0x8048703:	 "~~"
```

## fonction m

``` bash
(gdb) disas m
Dump of assembler code for function m:
   [...]
   0x08048501 <+13>:	call   0x80483d0 <time@plt>             # time(NULL)
   0x08048506 <+18>:	mov    edx,0x80486e0                    # "%s - %d\n"
   0x0804850b <+23>:	mov    DWORD PTR [esp+0x8],eax          # resultat de l'appel time
   0x0804850f <+27>:	mov    DWORD PTR [esp+0x4],0x8049960    # variable globale c == resultat du fgets
   0x08048517 <+35>:	mov    DWORD PTR [esp],edx              # 1er arg == "%s - %d\n"
   0x0804851a <+38>:	call   0x80483b0 <printf@plt>           # printf("%s - %d\n", c, timestamp);
   [...]
```

