// Пример сервера с использованием liburing
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <liburing.h>

#define PORT 12345
#define QUEUE_DEPTH 32

struct client {
    int fd;
};

void server_loop(struct io_uring *ring) {
    struct io_uring_sqe *sqe;
    struct io_uring_cqe *cqe;
    struct sockaddr_in addr;
    struct client clients[20];
    int server_fd, client_fd, ret;

    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT);
    addr.sin_addr.s_addr = INADDR_ANY;

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    bind(server_fd, (struct sockaddr*)&addr, sizeof(addr));
    listen(server_fd, 20);

    io_uring_accept(ring, server_fd, NULL, 0);

    while (1) {
        io_uring_submit(ring);

        ret = io_uring_wait_cqe(ring, &cqe);
        if (ret < 0) {
            fprintf(stderr, "io_uring_wait_cqe error: %s\n", strerror(-ret));
            exit(1);
        }

        sqe = io_uring_get_sqe(ring);

        if (cqe->res == 0) {
            // New connection
            client_fd = accept(server_fd, NULL, NULL);
            if (client_fd >= 0) {
                int i;
                for (i = 0; i < 20; ++i) {
                    if (clients[i].fd == 0) {
                        clients[i].fd = client_fd;
                        break;
                    }
                }
            }
            io_uring_accept(ring, server_fd, NULL, 0);
        } else {
            // Handle client data
            int i;
            for (i = 0; i < 20; ++i) {
                if (clients[i].fd == cqe->res) {
                    char buffer[1024];
                    recv(clients[i].fd, buffer, sizeof(buffer), 0);
                    printf("Received from client %d: %s\n", i, buffer);
                    break;
                }
            }
            io_uring_accept(ring, server_fd, NULL, 0);
        }

        io_uring_cqe_seen(ring, cqe);
    }
}

int main() {
    struct io_uring ring;
    int ret;

    ret = io_uring_queue_init(QUEUE_DEPTH, &ring, 0);
    if (ret < 0) {
        fprintf(stderr, "io_uring_queue_init error: %s\n", strerror(-ret));
        return 1;
    }

    server_loop(&ring);

    io_uring_queue_exit(&ring);

    return 0;
}
