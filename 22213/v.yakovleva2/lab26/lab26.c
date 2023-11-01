#include <stdio.h>
#define BUFSIZE 512
int main() {
    char line[BUFSIZE];

    FILE *fpin = popen("echo AbCdEfFfFtF!", "r");
    FILE *fpout = popen("tr [:lower:] [:upper:]", "w");

    while (fgets(line, BUFSIZE, fpin) != NULL) {
        fputs(line, fpout);
    }

    pclose(fpin);
    pclose(fpout);
}
