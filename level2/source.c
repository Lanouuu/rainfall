#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void p(void)
{
    char buffer[76];
    unsigned int saved_eip;

    fflush(stdout);

    gets(buffer);

    saved_eip = *(unsigned int *)(__builtin_frame_address(0) + 4);

    if ((saved_eip & 0xb0000000) == 0xb0000000)
    {
        printf("(%p)\n", saved_eip);
        _exit(1);
    }

    puts(buffer);
    strdup(buffer);
}

int main(void)
{
    p();

    return 0;
}