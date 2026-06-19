# level3
``` bash
=> 0x80484df <v+59>:	cmp    eax,0x40
(gdb) print 0x40
$1 = 64
```

       char *fgets(char s[restrict .size], int size, FILE *restrict stream);

```
void v(void)

{
  char local_20c [520];
  
  fgets(local_20c,0x200,stdin);
  printf(local_20c);
  if (m == 0x40) {
    fwrite("Wait what?!\n",1,0xc,stdout);
    system("/bin/sh");
  }
  return;
}
```

2*16*16
512

`display/i $pc`