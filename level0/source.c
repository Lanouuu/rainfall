#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char **argv)
{
    if (atoi(argv[1]) == 423)
    {
        char *args[] = {"/bin/sh", NULL};

        setresuid(geteuid(), geteuid(), geteuid());
        execv("/bin/sh", args);
    }
    else
    {
        fwrite("No !\n", 1, 5, stderr);
    }

    return 0;
}