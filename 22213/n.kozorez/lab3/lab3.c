#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void open_file() {
    FILE *file;
    file = fopen("file", "r");
    if (file == NULL) {
        perror("Couldn't open the file\n");
    }
    else {
        printf("File opened successfully\n");
        fclose(file);
    }
}

int main() {
    printf("User real id is %d\n", getuid());
    printf("User effective is %d\n", geteuid());

    open_file();

    if (setuid(getuid()) == -1) {
        perror("Couldn't change setuid\n");
        exit(1);
    }

    printf("User real id is %d\n", getuid());
    printf("User effective is %d\n", geteuid());

    open_file();

    exit(0);
}
