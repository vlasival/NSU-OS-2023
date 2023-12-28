#include <liburing.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <assert.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <sys/socket.h>

#define MAX_CONNECTIONS 10
#define BACKLOG 10
#define MAX_MESSAGE_LEN 1024
#define IORING_FEAT_FAST_POLL (1U << 5)

void add_accept(struct io_uring *ring, int fd, struct sockaddr *client_addr, socklen_t *client_len);

void add_socket_read(struct io_uring *ring, int fd, size_t size);

void add_socket_write(struct io_uring *ring, int fd, size_t size);

typedef struct conn_info {
    int fd;
    unsigned type;
} conn_info;

enum {
    ACCEPT,
    READ,
    WRITE,
};

conn_info conns[MAX_CONNECTIONS];
char bufs[MAX_CONNECTIONS][MAX_MESSAGE_LEN];

int main(int argc, char *argv[]) {
    int portno = strtol(argv[1], NULL, 10);
    struct sockaddr_in serv_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);

    int sock_listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    const int val = 1;
    setsockopt(sock_listen_fd, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val));

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(portno);
    serv_addr.sin_addr.s_addr = INADDR_ANY;

    assert(bind(sock_listen_fd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) >= 0);
    assert(listen(sock_listen_fd, BACKLOG) >= 0);

    struct io_uring_params params;
    struct io_uring ring;
    memset(&params, 0, sizeof(params));

    assert(io_uring_queue_init_params(4096, &ring, &params) >= 0);

    if (!(params.features & IORING_FEAT_FAST_POLL)) {
        printf("IORING_FEAT_FAST_POLL not available in the kernel, quiting...\n");
        exit(0);
    }

    add_accept(&ring, sock_listen_fd, (struct sockaddr *) &client_addr, &client_len);

    while (1) {
        struct io_uring_cqe *cqe;
        int ret;

        io_uring_submit(&ring);

        ret = io_uring_wait_cqe(&ring, &cqe);
        assert(ret == 0);

        struct io_uring_cqe *cqes[BACKLOG];
        int cqe_count = io_uring_peek_batch_cqe(&ring, cqes, sizeof(cqes) / sizeof(cqes[0]));

        for (int i = 0; i < cqe_count; ++i) {
            cqe = cqes[i];

            struct conn_info *user_data = (struct conn_info *) io_uring_cqe_get_data(cqe);

            unsigned type = user_data->type;
            if (type == ACCEPT) {
                int sock_conn_fd = cqe->res;

                add_socket_read(&ring, sock_conn_fd, MAX_MESSAGE_LEN);
                add_accept(&ring, sock_listen_fd, (struct sockaddr *) &client_addr, &client_len);
            } else if (type == READ) {
                int bytes_read = cqe->res;

                if (bytes_read <= 0) {
                    shutdown(user_data->fd, SHUT_RDWR);
                } else {
                    add_socket_write(&ring, user_data->fd, bytes_read);
                }
            } else if (type == WRITE) {
                add_socket_read(&ring, user_data->fd, MAX_MESSAGE_LEN);
            }

            io_uring_cqe_seen(&ring, cqe);
        }
    }
}

void add_accept(struct io_uring *ring, int fd, struct sockaddr *client_addr, socklen_t *client_len) {
    struct io_uring_sqe *sqe = io_uring_get_sqe(ring);
    io_uring_prep_accept(sqe, fd, client_addr, client_len, 0);

    conn_info *conn_i = &conns[fd];
    conn_i->fd = fd;
    conn_i->type = ACCEPT;

    io_uring_sqe_set_data(sqe, conn_i);
}

void add_socket_read(struct io_uring *ring, int fd, size_t size) {
    struct io_uring_sqe *sqe = io_uring_get_sqe(ring);
    io_uring_prep_recv(sqe, fd, &bufs[fd], size, 0);

    conn_info *conn_i = &conns[fd];
    conn_i->fd = fd;
    conn_i->type = READ;

    io_uring_sqe_set_data(sqe, conn_i);
}

void add_socket_write(struct io_uring *ring, int fd, size_t size) {
    struct io_uring_sqe *sqe = io_uring_get_sqe(ring);
    io_uring_prep_send(sqe, fd, &bufs[fd], size, 0);

    conn_info *conn_i = &conns[fd];
    conn_i->fd = fd;
    conn_i->type = WRITE;

    io_uring_sqe_set_data(sqe, conn_i);
}

