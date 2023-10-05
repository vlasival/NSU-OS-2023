#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <wait.h>

int main(int argc, char *argv[]) {
    pid_t child = fork();
    if (child == -1) {
        perror("Failed to fork");
        exit(1);
    } 
    if (child == 0) {
        if (execl("/bin/cat", "cat", "big_ass.txt", NULL) == -1) {
            perror("Failed to execute cat");
            exit(1);
        }
    } else {
        siginfo_t child_info;
        printf("%d\n", child);
        waitid(P_PID, child, &child_info, WEXITED); 
        printf("Cat ended.\n"); 
        printf(
            "signo=%d, errno=%d, code=%d, value=%d\n",
            child_info.si_signo,
            child_info.si_errno,
            child_info.si_code,
            child_info.si_value
        );
    };

    return 0;
}
