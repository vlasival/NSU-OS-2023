#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <liburing.h>

#define PORT 8080
#define BACKLOG 10
#define QUEUE_DEPTH 10

void server_start();

int main() {
    server_start();
    return 0;
}

void handle_client(struct io_uring *ring, int client_fd) {
    struct io_uring_sqe *sqe = io_uring_get_sqe(ring);
    io_uring_prep_poll_add(sqe, client_fd, POLL_IN);
    io_uring_submit(ring);

    struct io_uring_cqe *cqe;
    int ret = io_uring_wait_cqe(ring, &cqe);
    if (ret < 0) {
        perror("io_uring_wait_cqe");
        close(client_fd);
        return;
    }

    io_uring_cqe_seen(ring, cqe);

    char buffer[1024];
    ssize_t recv_size = recv(client_fd, buffer, sizeof(buffer), 0);
    if (recv_size < 0) {
        perror("recv");
        close(client_fd);
        return;
    }

    if (recv_size == 0) {
        close(client_fd);
        return;
    }

    buffer[recv_size] = '\0';
    printf("Received message from client: %s\n", buffer);
    close(client_fd);
}

void server_start() {
    struct io_uring ring;
    io_uring_queue_init(QUEUE_DEPTH, &ring, 0);

    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, BACKLOG) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %d...\n", PORT);

    while (1) {
        if ((new_socket = accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&addrlen)) < 0) {
            perror("accept");
            exit(EXIT_FAILURE);
        }

        handle_client(&ring, new_socket);
    }

    io_uring_queue_exit(&ring);
}

