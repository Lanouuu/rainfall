## ASM bonus0

## main

``` bash
(gdb) disas main
Dump of assembler code for function main:
   0x080485a4 <+0>:	push   ebp
   0x080485a5 <+1>:	mov    ebp,esp
   0x080485a7 <+3>:	and    esp,0xfffffff0
   0x080485aa <+6>:	sub    esp,0x40                          # Réservation de 64 octets sur la pile (pour char buffer[54])
   0x080485ad <+9>:	lea    eax,[esp+0x16]                    # Calcule l'adresse du buffer
   0x080485b1 <+13>:	mov    DWORD PTR [esp],eax               # 1er arg buffer
   0x080485b4 <+16>:	call   0x804851e <pp>                    # pp(buffer)
   0x080485b9 <+21>:	lea    eax,[esp+0x16]                    # Récupère l'adresse du buffer
   0x080485bd <+25>:	mov    DWORD PTR [esp],eax               # 1er arg buffer
   0x080485c0 <+28>:	call   0x80483b0 <puts@plt>              # puts(buffer)
   0x080485c5 <+33>:	mov    eax,0x0                           # return 0
   0x080485ca <+38>:	leave  
   0x080485cb <+39>:	ret
```

## Fonction pp

La fonction pp déclare deux buffers de 20 octets sur la pile et appelle `p` deux fois pour les remplir.

``` bash
0x0804851e <+0>:	push   ebp
0x0804851f <+1>:	mov    ebp,esp
0x08048521 <+3>:	push   edi
0x08048522 <+4>:	push   ebx
0x08048523 <+5>:	sub    esp,0x50                          # Réservation de 80 octets pour buffer1 et buffer2
0x08048526 <+8>:	mov    DWORD PTR [esp+0x4],0x80486a0     # 2e arg : " - " (prompt)
0x0804852e <+16>:	lea    eax,[ebp-0x30]                    # Calcule l'adresse de buffer1 (ebp-48)
0x08048531 <+19>:	mov    DWORD PTR [esp],eax               # 1er arg : buffer1
0x08048534 <+22>:	call   0x80484b4 <p>                     # p(buffer1, " - ")
0x08048539 <+27>:	mov    DWORD PTR [esp+0x4],0x80486a0     # 2e arg : " - " (prompt)
0x08048541 <+35>:	lea    eax,[ebp-0x1c]                    # Calcule l'adresse de buffer2 (ebp-28)
0x08048544 <+38>:	mov    DWORD PTR [esp],eax               # 1er arg : buffer2
0x08048547 <+41>:	call   0x80484b4 <p>                     # p(buffer2, " - ")
```

Ensuite buffer1 est copié dans le buffer de destination

``` bash
0x0804854c <+46>:	lea    eax,[ebp-0x30]                    # eax = buffer1
0x0804854f <+49>:	mov    DWORD PTR [esp+0x4],eax           # 2e arg : source (buffer1)
0x08048553 <+53>:	mov    eax,DWORD PTR [ebp+0x8]           # eax = dest (argument de pp)
0x08048556 <+56>:	mov    DWORD PTR [esp],eax               # 1er arg : destination (dest)
0x08048559 <+59>:	call   0x80483a0 <strcpy@plt>            # strcpy(dest, buffer1)
```

Le programme détermine la taille de dest sans passer par un strlen et ajoute un espace et le caractère nul à la fin de la chaine.

``` bash
0x0804855e <+64>:	mov    ebx,0x80486a4                     # ebx contient l'adresse de la chaîne " " (espace et \0)
0x08048563 <+69>:	mov    eax,DWORD PTR [ebp+0x8]           # eax = dest
0x08048566 <+72>:	mov    DWORD PTR [ebp-0x3c],0xffffffff   # Initialisation du compteur à -1
0x0804856d <+79>:	mov    edx,eax                           # Sauvegarde de dest
0x0804856f <+81>:	mov    eax,0x0                           # Cherche l'octet '\0'
0x08048574 <+86>:	mov    ecx,DWORD PTR [ebp-0x3c]          # ecx = -1
0x08048577 <+89>:	mov    edi,edx                           # edi = dest
0x08048579 <+91>:	repnz scas al,BYTE PTR es:[edi]          # Cherche '\0' et décrémente ecx
0x0804857b <+93>:	mov    eax,ecx
0x0804857d <+95>:	not    eax                               # Calcule la longueur de la chaîne
0x0804857f <+97>:	sub    eax,0x1
0x08048582 <+100>:	add    eax,DWORD PTR [ebp+0x8]        # eax = dest + len (pointe sur le '\0' final)
0x08048585 <+103>:	movzx  edx,WORD PTR [ebx]             # Charge 2 octets : ' ' (espace) et '\0'
0x08048588 <+106>:	mov    WORD PTR [eax],dx              # dest[len] = ' ', dest[len+1] = '\0'
```

Et il termine en utilisant strcat avec buffer2.

``` bash
0x0804858b <+109>:	lea    eax,[ebp-0x1c]                    # eax = buffer2
0x0804858e <+112>:	mov    DWORD PTR [esp+0x4],eax           # 2e arg : source (buffer2)
0x08048592 <+116>:	mov    eax,DWORD PTR [ebp+0x8]           # eax = dest
0x08048595 <+119>:	mov    DWORD PTR [esp],eax               # 1er arg : destination (dest)
0x08048598 <+122>:	call   0x8048390 <strcat@plt>            # strcat(dest, buffer2)
0x0804859d <+127>:	add    esp,0x50
0x080485a0 <+130>:	pop    ebx
0x080485a1 <+131>:	pop    edi
0x080485a2 <+132>:	pop    ebp
0x080485a3 <+133>:	ret
```

## Fonction p

La fonction p affiche son prompt, lit jusqu'à 4096 octest sur l'entrée standard avec read, retire le retour à la ligne, puis copie au plus 20 caractères dans son argument de destination avec strncpy.

``` bash
0x080484b4 <+0>:	push   ebp
0x080484b5 <+1>:	mov    ebp,esp
0x080484b7 <+3>:	sub    esp,0x1018                        # Réservation de 4120 octets (pour char buffer[4104])
0x080484bd <+9>:	mov    eax,DWORD PTR [ebp+0xc]           # eax = prompt
0x080484c0 <+12>:	mov    DWORD PTR [esp],eax               # 1er arg pour puts : prompt
0x080484c3 <+15>:	call   0x80483b0 <puts@plt>              # puts(prompt)
0x080484c8 <+20>:	mov    DWORD PTR [esp+0x8],0x1000        # 3e arg : count = 4096 (0x1000)
0x080484d0 <+28>:	lea    eax,[ebp-0x1008]                  # Calcule l'adresse du grand buffer (ebp-4104)
0x080484d6 <+34>:	mov    DWORD PTR [esp+0x4],eax           # 2e arg : buffer
0x080484da <+38>:	mov    DWORD PTR [esp],0x0               # 1er arg : fd = 0 (stdin)
0x080484e1 <+45>:	call   0x8048380 <read@plt>              # read(0, buffer, 4096)
0x080484e6 <+50>:	mov    DWORD PTR [esp+0x4],0xa           # 2e arg : '\n' (10 en ASCII)
0x080484ee <+58>:	lea    eax,[ebp-0x1008]                  # eax = buffer
0x080484f4 <+64>:	mov    DWORD PTR [esp],eax               # 1er arg : buffer
0x080484f7 <+67>:	call   0x80483d0 <strchr@plt>            # strchr(buffer, '\n')
0x080484fc <+72>:	mov    BYTE PTR [eax],0x0                # *strchr(buffer, '\n') = '\0'
0x080484ff <+75>:	lea    eax,[ebp-0x1008]                  # eax = buffer
0x08048505 <+81>:	mov    DWORD PTR [esp+0x8],0x14          # 3e arg : n = 20
0x0804850d <+89>:	mov    DWORD PTR [esp+0x4],eax           # 2e arg : source (buffer)
0x08048511 <+93>:	mov    eax,DWORD PTR [ebp+0x8]           # eax = dest
0x08048514 <+96>:	mov    DWORD PTR [esp],eax               # 1er arg : destination (dest)
0x08048517 <+99>:	call   0x80483f0 <strncpy@plt>           # strncpy(dest, buffer, 20)
0x0804851c <+104>:	leave  
0x0804851d <+105>:	ret
```