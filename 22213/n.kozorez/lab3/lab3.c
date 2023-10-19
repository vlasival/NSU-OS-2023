#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main()
{
    printf("User real id is %d\n", getuid());
    printf("User effective is %d\n", geteuid());

    FILE *file = fopen("file", "r");
    if (file == NULL)
    {
        perror("Couldn't open the file\n");
    }
    else
    {
        printf("File opened successfully\n");
        fclose(file);
    }

    if (setuid(getuid()) == -1)
    {
        perror("Couldn't change setuid\n");
        exit(1);
    }

    printf("User real id is %d\n", getuid());
    printf("User effective is %d\n", geteuid());

    file = fopen("file", "r");
    if (file == NULL)
    {
        perror("Couldn't open the file\n");
    }
    else
    {
        printf("File opened successfully\n");
        fclose(file);
    }

    exit(0);
}
