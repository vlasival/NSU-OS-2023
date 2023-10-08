#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

int main() {
    int waitstatus, pid;
    
    if ((pid = fork()) == - 1) {
            perror("failed to fork");
            exit(1);
    }
    if (pid == 0) {
        execl("/usr/bin/cat", "cat", "largefile", (char*) 0);

        perror("exec one failed"); 
        exit(1);
    }
    
    wait(&waitstatus);

    printf("task is completed!\n");
    exit(0);
}

