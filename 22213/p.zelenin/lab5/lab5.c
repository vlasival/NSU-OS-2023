#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#define BUF_LEN 2

int main(int argc, char* argv[]) {
    int fd, count = 0, count_n = 1;
    int capacity = BUF_LEN, capacity_n = BUF_LEN;
    int line;
    char c;
    ssize_t check;
    char* buf = (char*)malloc(sizeof(char) * BUF_LEN);
    int* buf_n = (int*)malloc(sizeof(int) * BUF_LEN);

    if (buf == NULL || buf_n == NULL) {
        perror("Memory allocation error - ");
        exit(EXIT_FAILURE);
    }
    buf_n[0] = 0;

    if (((fd = open(argv[1], O_RDONLY)) == -1)) {
        perror("Could not open the file - ");
        exit(EXIT_FAILURE);
    }

    while((check = read(fd, &c, 1)) > 0) {
        count++;
        if (count_n == capacity_n) {
            buf_n = (int*)realloc(buf_n, sizeof (int) * capacity_n * 2);
            if (buf_n == NULL) {
                perror("realloc() caused an error - ");
                exit(EXIT_FAILURE);
            }
            capacity_n *= 2;
        }
        if (c == '\n') {
            buf_n[count_n++] = lseek(fd, 0L, SEEK_CUR);
        }
    }
    if (check < 0) {
        perror("read() caused an error - ");
        exit(EXIT_FAILURE);
    }
    buf_n[count_n++] = lseek(fd, 0L, SEEK_CUR);

    if (capacity < count) {
        buf = (char*) realloc(buf, sizeof (char) * (count + 1));
        if (buf == NULL) {
            perror("realloc() caused an error - ");
            exit(EXIT_FAILURE);
        }
    }

    while (1) {
        lseek(fd, 0L, SEEK_SET);
        printf("Enter the line number - ");
        if (scanf("%d", &line) != 1) {
            perror("Could not read the line number - ");
            exit(EXIT_FAILURE);
        }
        if (line >= count_n || line < 0) {
            printf("There's no such line\n");
            continue;
        }
        if (line == 0) {
            break;
        }
        int len = buf_n[line] - buf_n[line - 1];
        lseek(fd, (long)buf_n[line - 1], 1);
        read(fd, buf, len);
        write(1, buf, len);
    }

    free(buf_n);
    free(buf);
    buf = NULL;
    buf_n = NULL;
    close(fd);
    exit(EXIT_SUCCESS);

}