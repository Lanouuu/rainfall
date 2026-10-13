## ASM level8

## Variables globales

Deux variables globales non initialisées dans le segment BSS (initialisées à 0 au démarrage) :

``` bash
~$ objdump -D level8
[...]
08049aac <auth>:
 8049aac:	00 00                	add    %al,(%eax)
	...

08049ab0 <service>:
 8049ab0:	00 00                	add    %al,(%eax)
	...
```

## Prologue et boucle principale

``` bash
(gdb) disas main
Dump of assembler code for function main:
   0x08048564 <+0>:    push   ebp
   0x08048565 <+1>:    mov    ebp,esp
   0x08048567 <+3>:    push   edi
   0x08048568 <+4>:    push   esi
   0x08048569 <+5>:    and    esp,0xfffffff0                    # alignement pile sur 16 octets
   0x0804856c <+8>:    sub    esp,0xa0                          # réserve 160 octets sur la pile (buffer[128] + variables locales)
   0x08048572 <+14>:   jmp    0x8048575 <main+17>               # saut au début du while(1) : le corps de la boucle commence à +17
   0x08048574 <+16>:   nop                                      # point de retour des jmp de fin de chaque if (continue du while)
```

## printf

``` bash
   0x08048575 <+17>:   mov    ecx,DWORD PTR ds:0x8049ab0        # charge la valeur de la globale service (pointeur ou NULL)
   0x0804857b <+23>:   mov    edx,DWORD PTR ds:0x8049aac        # charge la valeur de la globale auth (pointeur ou NULL)
   0x08048581 <+29>:   mov    eax,0x8048810                     # adresse de la chaîne de format "%p, %p \n"
   0x08048586 <+34>:   mov    DWORD PTR [esp+0x8],ecx           # 3e arg : service
   0x0804858a <+38>:   mov    DWORD PTR [esp+0x4],edx           # 2e arg : auth
   0x0804858e <+42>:   mov    DWORD PTR [esp],eax               # 1er arg : "%p, %p \n"
   0x08048591 <+45>:   call   0x8048410 <printf@plt>            # printf("%p, %p \n", auth, service)
```

## fgets

``` bash
   0x08048596 <+50>:   mov    eax,ds:0x8049a80                  # charge le pointeur FILE* stdin depuis la BSS
   0x0804859b <+55>:   mov    DWORD PTR [esp+0x8],eax           # 3e arg : stdin
   0x0804859f <+59>:   mov    DWORD PTR [esp+0x4],0x80          # 2e arg : taille max = 0x80 = 128 octets
   0x080485a7 <+67>:   lea    eax,[esp+0x20]                    # adresse du buffer local (esp+0x20 = esp+32, soit 160-32=128 octets disponibles)
   0x080485ab <+71>:   mov    DWORD PTR [esp],eax               # 1er arg : buffer
   0x080485ae <+74>:   call   0x8048440 <fgets@plt>             # fgets(buffer, 128, stdin)
   0x080485b3 <+79>:   test   eax,eax                           # fgets retourne NULL en cas d'EOF ou erreur
   0x080485b5 <+81>:   je     0x804872c <main+456>              # si NULL → break (sortie du while)
```

## strncmp "auth "

Le pattern `repz cmps` + `seta/setb/sub` est la façon dont le compilateur implémente `strncmp`.
`repz cmps` compare `esi` et `edi` octet par octet en décrémentant `ecx`, et s'arrête dès qu'un octet diffère ou que `ecx` atteint 0.
`seta`/`setb` capturent le résultat de la dernière comparaison (supérieur / inférieur non signé).
`sub cl,al` calcule `(supérieur ? 1 : 0) - (inférieur ? 1 : 0)` → 0 si égal, non nul sinon.

``` bash
   0x080485bb <+87>:   lea    eax,[esp+0x20]                    # adresse du buffer
   0x080485bf <+91>:   mov    edx,eax                           # edx = buffer
   0x080485c1 <+93>:   mov    eax,0x8048819                     # eax = adresse de la chaîne "auth "
   0x080485c6 <+98>:   mov    ecx,0x5                           # ecx = 5 : nombre de caractères à comparer
   0x080485cb <+103>:  mov    esi,edx                           # esi = buffer (1er opérande de cmps)
   0x080485cd <+105>:  mov    edi,eax                           # edi = "auth " (2e opérande de cmps)
   0x080485cf <+107>:  repz cmps BYTE PTR ds:[esi],BYTE PTR es:[edi]  # compare 5 octets : buffer vs "auth ", s'arrête si inégal
   0x080485d1 <+109>:  seta   dl                                # dl = 1 si buffer[i] > "auth "[i] (non signé)
   0x080485d4 <+112>:  setb   al                                # al = 1 si buffer[i] < "auth "[i] (non signé)
   0x080485d7 <+115>:  mov    ecx,edx
   0x080485d9 <+117>:  sub    cl,al                             # cl = (supérieur) - (inférieur) → 0 si égal
   0x080485db <+119>:  mov    eax,ecx
   0x080485dd <+121>:  movsx  eax,al                            # signe-extend le résultat en 32 bits
   0x080485e0 <+124>:  test   eax,eax                           # résultat == 0 ?
   0x080485e2 <+126>:  jne    0x8048642 <main+222>              # si différent de "auth " → saute au bloc suivant (reset)
```

Si le buffer commence par `"auth "`, on entre dans le bloc :

``` bash
   0x080485e4 <+128>:  mov    DWORD PTR [esp],0x4               # 1er arg : taille = 4 octets
   0x080485eb <+135>:  call   0x8048470 <malloc@plt>            # auth = malloc(4)
   0x080485f0 <+140>:  mov    ds:0x8049aac,eax                  # stocke le pointeur retourné dans la globale auth
   0x080485f5 <+145>:  mov    eax,ds:0x8049aac                  # recharge auth dans eax
   0x080485fa <+150>:  mov    DWORD PTR [eax],0x0               # *auth = 0 (initialisation du premier mot à zéro)
```

Calcul du strlen de `buffer+5` (pattern `repnz scas` = strlen en assembleur) :

``` bash
   0x08048600 <+156>:  lea    eax,[esp+0x20]                    # adresse du buffer
   0x08048604 <+160>:  add    eax,0x5                           # buffer+5 : pointe après "auth "
   0x08048607 <+163>:  mov    DWORD PTR [esp+0x1c],0xffffffff   # compteur initialisé à -1 (0xffffffff)
   0x0804860f <+171>:  mov    edx,eax                           # sauvegarde buffer+5
   0x08048611 <+173>:  mov    eax,0x0                           # AL = 0 : octet cherché = '\0'
   0x08048616 <+178>:  mov    ecx,DWORD PTR [esp+0x1c]          # ecx = 0xffffffff (compteur pour repnz)
   0x0804861a <+182>:  mov    edi,edx                           # edi = buffer+5 (repnz scas lit depuis edi)
   0x0804861c <+184>:  repnz scas al,BYTE PTR es:[edi]          # avance edi octet par octet jusqu'au '\0', décrémente ecx à chaque pas
   0x0804861e <+186>:  mov    eax,ecx
   0x08048620 <+188>:  not    eax
   0x08048622 <+190>:  sub    eax,0x1
   0x08048625 <+193>:  cmp    eax,0x1e                          # compare strlen avec 30 (0x1e)
   0x08048628 <+196>:  ja     0x8048642 <main+222>              # si strlen > 30 → skip le strcpy
```

Si `strlen(buffer+5) <= 30`, on copie dans auth :

``` bash
   0x0804862a <+198>:  lea    eax,[esp+0x20]                    # adresse du buffer
   0x0804862e <+202>:  lea    edx,[eax+0x5]                     # edx = buffer+5 (source : la chaîne après "auth ")
   0x08048631 <+205>:  mov    eax,ds:0x8049aac                  # eax = auth (destination)
   0x08048636 <+210>:  mov    DWORD PTR [esp+0x4],edx           # 2e arg : src = buffer+5
   0x0804863a <+214>:  mov    DWORD PTR [esp],eax               # 1er arg : dst = auth
   0x0804863d <+217>:  call   0x8048460 <strcpy@plt>            # strcpy(auth, buffer+5)
```

## strncmp "reset"

Même pattern `repz cmps` que pour "auth ", avec ecx=5 et la chaîne "reset" :

``` bash
   0x08048642 <+222>:  lea    eax,[esp+0x20]                    # adresse du buffer
   0x08048646 <+226>:  mov    edx,eax                           # edx = buffer
   0x08048648 <+228>:  mov    eax,0x804881f                     # eax = adresse de la chaîne "reset"
   0x0804864d <+233>:  mov    ecx,0x5                           # ecx = 5 : 5 caractères à comparer
   0x08048652 <+238>:  mov    esi,edx                           # esi = buffer
   0x08048654 <+240>:  mov    edi,eax                           # edi = "reset"
   0x08048656 <+242>:  repz cmps BYTE PTR ds:[esi],BYTE PTR es:[edi]  # compare 5 octets : buffer vs "reset"
   0x08048658 <+244>:  seta   dl                                # dl = 1 si buffer[i] > "reset"[i]
   0x0804865b <+247>:  setb   al                                # al = 1 si buffer[i] < "reset"[i]
   0x0804865e <+250>:  mov    ecx,edx
   0x08048660 <+252>:  sub    cl,al                             # cl = 0 si égal
   0x08048662 <+254>:  mov    eax,ecx
   0x08048664 <+256>:  movsx  eax,al
   0x08048667 <+259>:  test   eax,eax                           # résultat == 0 ?
   0x08048669 <+261>:  jne    0x8048678 <main+276>              # si différent de "reset" -> saute au bloc suivant (service)
   0x0804866b <+263>:  mov    eax,ds:0x8049aac                  # eax = auth (pointeur global)
   0x08048670 <+268>:  mov    DWORD PTR [esp],eax               # 1er arg : auth
   0x08048673 <+271>:  call   0x8048420 <free@plt>              # free(auth) — auth pointe toujours vers la zone libérée après cet appel (use-after-free possible)
```

## strncmp "service"

``` bash
   0x08048678 <+276>:  lea    eax,[esp+0x20]                    # adresse du buffer
   0x0804867c <+280>:  mov    edx,eax                           # edx = buffer
   0x0804867e <+282>:  mov    eax,0x8048825                     # eax = adresse de la chaîne "service"
   0x08048683 <+287>:  mov    ecx,0x6                           # ecx = 6 : 6 caractères à comparer ("servic")
   0x08048688 <+292>:  mov    esi,edx                           # esi = buffer
   0x0804868a <+294>:  mov    edi,eax                           # edi = "service"
   0x0804868c <+296>:  repz cmps BYTE PTR ds:[esi],BYTE PTR es:[edi]  # compare 6 octets : buffer vs "service"
   0x0804868e <+298>:  seta   dl                                # dl = 1 si buffer[i] > "service"[i]
   0x08048691 <+301>:  setb   al                                # al = 1 si buffer[i] < "service"[i]
   0x08048694 <+304>:  mov    ecx,edx
   0x08048696 <+306>:  sub    cl,al                             # cl = 0 si égal
   0x08048698 <+308>:  mov    eax,ecx
   0x0804869a <+310>:  movsx  eax,al                            # signe-extend
   0x0804869d <+313>:  test   eax,eax                           # résultat == 0 ?
   0x0804869f <+315>:  jne    0x80486b5 <main+337>              # si différent de "service" → saute au bloc suivant (login)
   0x080486a1 <+317>:  lea    eax,[esp+0x20]                    # adresse du buffer
   0x080486a5 <+321>:  add    eax,0x7                           # buffer+7 : pointe après "service"
   0x080486a8 <+324>:  mov    DWORD PTR [esp],eax               # 1er arg : buffer+7
   0x080486ab <+327>:  call   0x8048430 <strdup@plt>            # service = strdup(buffer+7) : alloue sur le heap et copie
   0x080486b0 <+332>:  mov    ds:0x8049ab0,eax                  # stocke le nouveau pointeur dans la globale service
```

## strncmp "login"

``` bash
   0x080486b5 <+337>:  lea    eax,[esp+0x20]                    # adresse du buffer
   0x080486b9 <+341>:  mov    edx,eax                           # edx = buffer
   0x080486bb <+343>:  mov    eax,0x804882d                     # eax = adresse de la chaîne "login"
   0x080486c0 <+348>:  mov    ecx,0x5                           # ecx = 5 : 5 caractères à comparer
   0x080486c5 <+353>:  mov    esi,edx                           # esi = buffer
   0x080486c7 <+355>:  mov    edi,eax                           # edi = "login"
   0x080486c9 <+357>:  repz cmps BYTE PTR ds:[esi],BYTE PTR es:[edi]  # compare 5 octets : buffer vs "login"
   0x080486cb <+359>:  seta   dl                                # dl = 1 si buffer[i] > "login"[i]
   0x080486ce <+362>:  setb   al                                # al = 1 si buffer[i] < "login"[i]
   0x080486d1 <+365>:  mov    ecx,edx
   0x080486d3 <+367>:  sub    cl,al                             # cl = 0 si égal
   0x080486d5 <+369>:  mov    eax,ecx
   0x080486d7 <+371>:  movsx  eax,al                            # signe-extend
   0x080486da <+374>:  test   eax,eax                           # résultat == 0 ?
   0x080486dc <+376>:  jne    0x8048574 <main+16>               # si différent de "login" → retour au début du while (nop à +16)
```

Si le buffer commence par `"login"`, on vérifie `*(auth + 0x20)` :

``` bash
   0x080486e2 <+382>:  mov    eax,ds:0x8049aac                  # eax = auth (pointeur global vers la zone malloc)
   0x080486e7 <+387>:  mov    eax,DWORD PTR [eax+0x20]          # lit l'octet à auth+32 (0x20 = 32 octets au-delà du début du malloc)
   0x080486ea <+390>:  test   eax,eax                           # est-ce que auth+32 vaut 0 ?
   0x080486ec <+392>:  je     0x80486ff <main+411>              # si 0 mot de passe incorrect (fwrite "Password:\n")
   0x080486ee <+394>:  mov    DWORD PTR [esp],0x8048833         # 1er arg : "/bin/sh"
   0x080486f5 <+401>:  call   0x8048480 <system@plt>            # system("/bin/sh") → shell avec droits level9
   0x080486fa <+406>:  jmp    0x8048574 <main+16>               # retour au début du while
```

Sinon, affichage de `"Password:\n"` :

``` bash
   0x080486ff <+411>:  mov    eax,ds:0x8049aa0                  # charge stdout depuis la BSS
   0x08048704 <+416>:  mov    edx,eax                           # edx = stdout
   0x08048706 <+418>:  mov    eax,0x804883b                     # eax = adresse de la chaîne "Password:\n"
   0x0804870b <+423>:  mov    DWORD PTR [esp+0xc],edx           # 4e arg : stdout
   0x0804870f <+427>:  mov    DWORD PTR [esp+0x8],0xa           # 3e arg : 10 octets à écrire (0xa = 10)
   0x08048717 <+435>:  mov    DWORD PTR [esp+0x4],0x1           # 2e arg : taille d'un élément = 1 octet
   0x0804871f <+443>:  mov    DWORD PTR [esp],eax               # 1er arg : "Password:\n"
   0x08048722 <+446>:  call   0x8048450 <fwrite@plt>            # fwrite("Password:\n", 1, 10, stdout)
   0x08048727 <+451>:  jmp    0x8048574 <main+16>               # retour au début du while
```

