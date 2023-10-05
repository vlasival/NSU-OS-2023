#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include <stdio.h>

int main()
{
    time_t t = time(0);
    if (putenv("TZ=US/Pacific")) {
        fprintf(stderr, "putenv() function caused an error: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }    
    if (localtime(&t) == NULL) {
        fprintf(stderr, "localtime() function caused an error: %s\n", strerror(errno));
        exit(EXIT_FAILURE);  
    }
    if (asctime(localtime(&t)) == NULL) {
        fprintf(stderr, "asctime() function caused an error: %s\n", strerror(errno));
        exit(EXIT_FAILURE); 
    }
                    
    printf("%s/n", asctime(localtime(&t)));
    exit(0);
}
