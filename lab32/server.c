#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <arpa/inet.h>
#include <errno.h>

#define SOCKET_PATH "/tmp/socket_server"
#define MAX_CLIENTS 10

int server_fd;

typedef struct {
    int fd;
    char buffer[256];
    ssize_t bytes_to_read;
} Client;

void handle_client_data(Client* client) {
    for (ssize_t i = 0; i < client->bytes_to_read; ++i) {
        client->buffer[i] = toupper(client->buffer[i]);
    }

    printf("Received from client %d: %.*s\n", client->fd, (int)client->bytes_to_read, client->buffer);

    write(client->fd, client->buffer, client->bytes_to_read);

    memset(client->buffer, 0, sizeof(client->buffer));
    client->bytes_to_read = 0;
}

void stop_server() {
    printf("Closing server.\n");
    close(server_fd);
    unlink(SOCKET_PATH);
    exit(0);
}

int main() {
    int client_fd;
    struct sockaddr_un server_addr, client_addr;
    socklen_t client_len;

    // Create server socket
    server_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (server_fd == -1) {
        perror("Error creating server socket");
        exit(EXIT_FAILURE);
    }

    // Set up server address structure
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sun_family = AF_UNIX;
    strncpy(server_addr.sun_path, SOCKET_PATH, sizeof(server_addr.sun_path) - 1);

    // Bind server socket to the address
    if (bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("Error binding server socket");
        unlink(SOCKET_PATH);
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    if (listen(server_fd, 5) == -1) {
        perror("Error listening for connections");
        unlink(SOCKET_PATH);
        exit(EXIT_FAILURE);
    }

    printf("Server is listening on %s\n", SOCKET_PATH);

    // Set the server socket to non-blocking mode
    fcntl(server_fd, F_SETFL, fcntl(server_fd, F_GETFL) | O_NONBLOCK);

    // Create an array to store client data
    Client clients[MAX_CLIENTS] = {{0}};
    
    signal(SIGINT, stop_server);

    while (1) {
        // Accept incoming connections
        client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &client_len);
        if (client_fd > 0) {
            // Set the client socket to non-blocking mode
            fcntl(client_fd, F_SETFL, fcntl(client_fd, F_GETFL) | O_NONBLOCK);

            // Find an empty slot for the new client
            int i;
            for (i = 0; i < MAX_CLIENTS; ++i) {
                if (clients[i].fd == 0) {
                    clients[i].fd = client_fd;
                    break;
                }
            }

            if (i == MAX_CLIENTS) {
                fprintf(stderr, "Too many clients, closing connection\n");
                close(client_fd);
            }
            printf("Client %d connected.\n", clients[i].fd);
        }

        // Check data from clients
        for (int i = 0; i < MAX_CLIENTS; ++i) {
            if (clients[i].fd > 0) {
                ssize_t bytesRead = read(clients[i].fd, clients[i].buffer, sizeof(clients[i].buffer) - 1);
                if (bytesRead > 0) {
                    clients[i].bytes_to_read = bytesRead;
                    handle_client_data(&clients[i]);
                } else if (bytesRead == 0 || (bytesRead == -1 && errno != EWOULDBLOCK && errno != EAGAIN)) {
                    // Connection closed by the client or error occurred
                    printf("Client %d disconnected\n", clients[i].fd);
                    close(clients[i].fd);
                    clients[i].fd = 0;
                }
            }
        }
    }

    return 0;
}

