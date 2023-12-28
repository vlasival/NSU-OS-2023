#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <liburing.h>

#define PORT 8080
#define MAX_CLIENTS 10
#define QUEUE_DEPTH 10

int server_fd;

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

void stop_server() {
    printf("Closing server.\n");
    close(server_fd);
    io_uring_queue_exit(&ring);
    exit(0);
}

void server_start() {
    struct io_uring ring;
    io_uring_queue_init(QUEUE_DEPTH, &ring, 0);

    int clients[MAX_CLIENTS] = {0};
    // ipV4 address
    struct sockaddr_in address;
    // setsockopt 
    int opt = 1;
    int addrlen = sizeof(address);

    // ipV4 TCP socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) { 
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // set socket options
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    // configure address
    // set type ipV4
    address.sin_family = AF_INET;
    // listen all ips
    address.sin_addr.s_addr = INADDR_ANY;
    // set port
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, MAX_CLIENTS) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %d\n", PORT);

    int flags = fcntl(server_fd, F_GETFL, 0);
    fcntl(server_fd, F_SETFL, flags | O_NONBLOCK);

    signal(SIGINT, stop_server);

    while (1) {
        
        if ((new_socket = accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&addrlen)) < 0) {
            perror("accept");
            exit(EXIT_FAILURE);
        }

        if (new_socket > 0) {
            int i;
            for (i = 0; i < MAX_CLIENTS; i++) {
                if (clients[i] == 0){
                    break;
                }
            }
            if (i == MAX_CLIENTS) {
                printf("Too much clients\n");
                close(new_socket);
                continue;
            }
        }

        handle_client(&ring, new_socket);
    }
}

int main() {
    server_start();
    return 0;
}
