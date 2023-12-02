#include <sys/mman.h>
#include <fcntl.h>

int main() {
    int fd = open("data.txt", O_RDONLY);
    off_t file_size = lseek(fd, 0, SEEK_END);

    void *mapped_data = mmap(NULL, file_size, PROT_READ, MAP_PRIVATE, fd, 0);

    munmap(mapped_data, file_size);
    close(fd);

    return 0;
}
