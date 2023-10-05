#include <sys/types.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
int main()
{
    int code = putenv("TZ=America/Los_Angeles");
    if (code != 0)
    {
        perror("putenv error");
        exit(1);
    }
    time_t date = time(NULL);
    if (date == -1)
    {
        perror("time error");
        exit(1);
    }

    char* time = ctime(&date);

    if (time == NULL)
    {
        perror("ctime error");
        exit(1);
    }
    printf("%s", time);
    exit(0);
}
