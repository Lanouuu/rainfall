#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

char c[80];

void m()
{
    time_t timestamp;

    timestamp = time(NULL);
    printf("%s - %d\n", c, timestamp);
}

int main(int argc, char **argv)
{
    int *data1;
    int *data2;
    FILE *file;

    data1 = malloc(8);
    data1[0] = 1;
    data1[1] = (int)malloc(8);                      // buffer1

    data2 = malloc(8);
    data2[0] = 2;
    data2[1] = (int)malloc(8);                      // buffer2

    strcpy((char *)data1[1], argv[1]);
    strcpy((char *)data2[1], argv[2]);

    file = fopen("/home/user/level8/.pass", "r");
    fgets(c, 68, file);

    puts("~~");

    return 0;
}

