#include <stdio.h>
#include <string.h>
#include <unistd.h>

void p(char *dest, char *prompt)
{
    char buffer[4104];

    puts(prompt);
    read(0, buffer, 4096);
    *strchr(buffer, '\n') = '\0';
    strncpy(dest, buffer, 20);
}

void pp(char *dest)
{
    char buffer1[20];
    char buffer2[20];
    size_t len;

    p(buffer1, " - ");
    p(buffer2, " - ");

    strcpy(dest, buffer1);

    len = strlen(dest);

    dest[len] = ' ';
    dest[len + 1] = '\0';

    strcat(dest, buffer2);
}

int main(void)
{
    char buffer[54];

    pp(buffer);
    puts(buffer);
    return 0;
}
