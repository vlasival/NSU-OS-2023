#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <ctype.h>
#include <unistd.h>
#include <string.h>

#define   MSGSIZE   1

int main() {
    int fd[2], status;
    pid_t pid;
    char msgout[100] = "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA\nAPSLPASPF";
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
            while (status > 0) {
                for (int i = 0; i < MSGSIZE; i++) {
                    msgin[i] = toupper(msgin[i]);
                }
                if ((write(0, msgin, MSGSIZE)) == -1) {
                    perror("Write failure");
                    exit(EXIT_FAILURE);
                };
                status = read(fd[0], msgin, MSGSIZE);
            }
            
            close(fd[0]);
            break;
        default:
            close(fd[0]);
            if ((status = write(fd[1], msgout, strlen(msgout))) == -1) {
                perror("Write failure");
                exit(EXIT_FAILURE);
            };
            close(fd[1]);
    }

    exit(EXIT_SUCCESS);
}
