#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>

int main() {
    FILE *file;
    uid_t euid = geteuid();
    printf("Real user id: %u\n", getuid());
    printf("Effective user id: %u\n\n", euid);

    if ((file = fopen("example.txt", "w")) == NULL) {
        perror("fopen: ");
        exit(1);
    }


    if (setuid(euid) < 0) {
        perror("setuid: ");
        exit(1);
    }

    printf("Real user id: %u\n", getuid());
    printf("Effective user id: %u\n", geteuid());

    fclose(file);
    exit(0);
}