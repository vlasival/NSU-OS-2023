#include <sys/types.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
int main()
{
    int code = putenv("TZ=America/Los_Angeles");
    if (code < 0)
    {
        perror("putenv error");
        exit(0);
    }
    time_t date = time(NULL);
    if (date == -1)
    {
        perror("time error");
        exit(0);
    }

    if (ctime(&date) == NULL)
    {
        perror("ctime error");
        exit(0);
    }
    printf("%s", ctime(&date));
    exit(0);
}
