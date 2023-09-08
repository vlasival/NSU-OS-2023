#include <stdio.h>
#include <time.h>
#include <stdlib.h>
 
int main()
{
        time_t now = time(NULL);
        putenv("TZ=GMT+7");
        fprintf(stdout, "%s\n", ctime(&now));
        return 0;
}

