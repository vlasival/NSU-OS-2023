#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <ctype.h>
#include <unistd.h>

#define   MSGSIZE   20

int main(int argc, char **argv) {
    int fd[2], status;
    pid_t pid;
    char msgout[MSGSIZE] = "Some Text\n";
    char msgin[MSGSIZE];

    if (pipe(fd) == -1) {
        perror("Pipe creation failure");
        exit(EXIT_FAILURE);
    }
    
    switch (pid = fork()) {
        case -1:
            perror("Fork failure");
            exit(EXIT_FAILURE);
        case 0:
            close(fd[1]);
            if ((status = read(fd[0], msgin, MSGSIZE)) == -1) {
                perror("Read failure");
                exit(EXIT_FAILURE);
            };
            close(fd[0]);

            for (int i = 0; i < MSGSIZE; i++) {
                msgin[i] = toupper(msgin[i]);
            }
            
            if ((status = write(0, msgin, MSGSIZE)) == -1) {
                perror("Write failure");
                exit(EXIT_FAILURE);
            };
            break;
        default:
            close(fd[0]);
            if ((status = write(fd[1], msgout, MSGSIZE)) == -1) {
                perror("Write failure");
                exit(EXIT_FAILURE);
            };
            close(fd[1]);
    }

    exit(EXIT_SUCCESS);
}
