#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <liburing.h>

#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 8080

void send_message(struct io_uring *ring, int socket_fd, const char *message);

int main() {
    struct io_uring ring;
    io_uring_queue_init(1, &ring, 0);

    int socket_fd;
    struct sockaddr_in server_address;

    if ((socket_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket creation failure");
        exit(EXIT_FAILURE);
    }

    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(SERVER_PORT);
    
    if (inet_pton(AF_INET, SERVER_IP, &server_address.sin_addr) <= 0) {
        perror("invalid address");
        exit(EXIT_FAILURE);
    }

    if (connect(socket_fd, (struct sockaddr*)&server_address, sizeof(server_address)) < 0) {
        perror("connection failure");
        exit(EXIT_FAILURE);
    }

    const char *message = "Hello from the client!";
    send_message(&ring, socket_fd, message);

    io_uring_queue_exit(&ring);
    close(socket_fd);

    return 0;
}

void send_message(struct io_uring *ring, int socket_fd, const char *message) {
    struct io_uring_sqe *sqe = io_uring_get_sqe(ring);
    io_uring_prep_send(sqe, socket_fd, message, strlen(message), 0);
    io_uring_submit(ring);

    struct io_uring_cqe *cqe;
    int ret = io_uring_wait_cqe(ring, &cqe);
    if (ret < 0) {
        perror("io_uring_wait_cqe");
        close(socket_fd);
        exit(EXIT_FAILURE);
    }

    io_uring_cqe_seen(ring, cqe);
}

