#include <sys/socket.h>
#include <sys/un.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <strings.h>

#define CLIENT_SOCK_FILE "client.sock"
#define SERVER_SOCK_FILE "server.sock"

int main() {
    char buf[100];
    int fd, rc;

    if ((fd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1){
        perror("socket creating error");
        exit(1);
    }

    struct sockaddr_un addr;
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strcpy(addr.sun_path, SERVER_SOCK_FILE);

    if (connect(fd, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
        perror("connection failure");
        exit(1);
    }

    while ((rc = read(STDIN_FILENO, buf, sizeof(buf))) > 0) {
        if ((write(fd, buf, rc) != rc)) {
            if (rc > 0) {
                perror("partial write");
            } else {
                perror("write error");
                exit(1);
            }
        }
    }

    return 0;

}