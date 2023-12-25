#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/socket.h>
#include <sys/un.h>

#define SOCKET_PATH "/tmp/socket_server"

int client_fd;

void stop_client() {
    printf("Closing client.\n");
    close(client_fd);
    exit(EXIT_SUCCESS);
}

int main() {
    struct sockaddr_un server_addr;
    char buffer[256];

    // Create client socket
    client_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (client_fd == -1) {
        perror("Error creating client socket");
        exit(EXIT_FAILURE);
    }

    // Set up server address structure
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sun_family = AF_UNIX;
    strncpy(server_addr.sun_path, SOCKET_PATH, sizeof(server_addr.sun_path) - 1);

    // Connect to the server
    if (connect(client_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("Error connecting to the server");
        exit(EXIT_FAILURE);
    }

    signal(SIGINT, stop_client);

    while (1) {
        // Read input from the user
        printf("> ");
        fgets(buffer, sizeof(buffer), stdin);

        // Send input to the server
        write(client_fd, buffer, strlen(buffer));

        // Read response from the server
        ssize_t bytesRead = read(client_fd, buffer, sizeof(buffer) - 1);
        if (bytesRead > 0) {
            // Print converted text received from the server
            printf("Received from server: %.*s\n", (int)bytesRead, buffer);
        } else if (bytesRead == 0) {
            // Server closed the connection
            printf("Server closed the connection\n");
            break;
        } else {
            perror("Error reading from server");
            break;
        }
    }

    return 0;
}

