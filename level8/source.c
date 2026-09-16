#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *auth;
char *service;

int main(void)
{
    char buffer[128];

    while (1)
    {
        printf("%p, %p \n", auth, service);

        if (fgets(buffer, sizeof(buffer), stdin) == NULL)
            break;

        if (strncmp(buffer, "auth ", 5) == 0)
        {
            auth = malloc(4);
            *auth = 0;

            if (strlen(buffer + 5) <= 30)
                strcpy(auth, buffer + 5);
        }

        if (strncmp(buffer, "reset", 5) == 0)
        {
            free(auth);
        }

        if (strncmp(buffer, "service", 7) == 0)
        {
            service = strdup(buffer + 7);
        }

        if (strncmp(buffer, "login", 5) == 0)
        {
            if (*(auth + 0x20) != 0)
                system("/bin/sh");
            else
                fwrite("Password:\n", 1, 10, stdout);
        }
    }

    return 0;
}