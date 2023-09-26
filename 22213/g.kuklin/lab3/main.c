#include <stdio.h>
#include <errno.h>
#include <ucred.h>
#include <unistd.h>
#include <stdlib.h>

void check_file(const char *file) {
    FILE *file = fopen(file, O_RDONLY);
    if (file == NULL) {
        perror("Failed to open file");
        return;
    }
    if (fclose(file)) {
        perror("Failed to close file");    
    };
}

int main(int argc, char *argv[]) {
    const char *filename = "test_file.txt";
    printf("Effective uid: %d. Real uid: %d.\n", geteuid(), getuid());

    check_file(filename);

    uid_t my_uid = getuid();
    if (setuid(my_uid)) {
        perror("Failed to set euid");    
    };
    printf("Effective uid: %d. Real uid: %d.\n", geteuid(), getuid());

    check_file(filename);

    exit(0);
}
