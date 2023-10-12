#include <unistd.h>
#include <sys/wait.h>
#include <stdio.h>

int main(int argc, char *argv[]) {
    if (argc == 1) {
        return 1;
    }

    pid_t pid = fork();
    switch (pid) {
        case -1: return 1;
        case 0:
        {
            execvp(argv[1], argv + 1);
            return 0;
        }
        default:
        {
            int stat_loc;
            siginfo_t info;
            waitid(P_PID, pid, &info, WEXITED);

            if (info.si_code == CLD_EXITED) {
                printf("\nChild process finished with exit code %d\n",
                       info.si_status);
            }
        }
    }

    return 0;
}


