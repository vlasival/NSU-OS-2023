#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <curses.h>
#include <unistd.h>


int cnt = 0;


void beep_handler();
void quit();

int main() {
    struct sigaction beep_sigaction;
    beep_sigaction.sa_handler = beep_handler;

    sigaction(SIGINT, &beep_sigaction, NULL);
    signal(SIGQUIT, quit);

    while (1);
}

void beep_handler() {
    write(1, "\a", 1);
    cnt++;
}

void quit() {
    char buf[11];
    buf[0] = '0';
    int step = 1000000000, i = 9, to_write = 1;
    if (cnt > 0)
    {
        while (cnt / step == 0 && step > 0) {
            step /= 10;
            i--;
        }
        to_write = 0;
        while (i >= 0) {
            buf[i] = (cnt - (cnt / 10) * 10) + '0';
            cnt /= 10;
            i--;
            to_write++;
        }
    }
    
    write(1, "The sound was produced ", 23);
    write(1, buf, to_write);
    write(1, " times\n", 7);

    exit(EXIT_SUCCESS);
}
