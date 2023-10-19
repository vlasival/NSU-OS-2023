#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

int main() {
    int waitstatus;
    pid_t pid;
    
    if ((pid = fork()) == - 1) {
            perror("failed to fork");
            exit(1);
    }
    if (pid == 0) {
        if (execlp("cat", "cat", "largefile", (char*) 0) == -1)
            perror("failed to run cat");

        perror("exec one failed"); 
        exit(1);
    }
   
    if (waitid(P_PID, pid, NULL, WEXITED) == -1) {
        perror("failed to wait child process");
        exit(1);
    }

    printf("task is completed!\n");
    exit(0);
}

