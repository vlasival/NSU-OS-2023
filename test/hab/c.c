#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>

#define SERVER_PORT 12345

int main() {
    int sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_fd < 0) {
        perror("Error creating socket");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(SERVER_PORT);
    serv_addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

    if (connect(sock_fd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("Error connecting to the server");
        close(sock_fd);
        exit(EXIT_FAILURE);
    }

    char message[1024];

    // Читаем строки из stdin и отправляем на сервер
    while (1) {
        printf("Enter a message (or 'exit' to quit): ");
        if (fgets(message, sizeof(message), stdin) == NULL) {
            perror("Error reading from stdin");
            close(sock_fd);
            exit(EXIT_FAILURE);
        }

        // Убираем символ новой строки, если есть
        size_t len = strlen(message);
        if (len > 0 && message[len - 1] == '\n') {
            message[len - 1] = '\0';
        }

        // Проверка на выход
        if (strcmp(message, "exit") == 0) {
            break;
        }

        // Отправляем сообщение на сервер
        ssize_t bytes_sent = send(sock_fd, message, strlen(message), 0);
        if (bytes_sent < 0) {
            perror("Error sending message");
            close(sock_fd);
            exit(EXIT_FAILURE);
        }
    }

    close(sock_fd);
    return 0;
}

