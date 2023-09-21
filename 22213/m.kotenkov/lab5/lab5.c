#include <stdio.h>
#include <stdlib.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>


typedef struct str_s {
    int offset;
    int len;
} str;

int main() {

    int fd = open("./big_text.txt", O_RDONLY);
    if (fd == -1)
    {
        perror("File is not opened.");
        exit(1);
    }
    
    int str_len = 0,
        pos = 0,
        str_cnt = 0,
        choice,
        allocated = 1,
        last = 0;
    
    char buffer[BUFSIZ];

    str *tmp;
    str *matrix;
    if ((matrix = (str*) malloc(sizeof(str))) == NULL)
    {
        perror("Malloc error");
        exit(2);
    }
    
    
    ssize_t bytes_red;

    bytes_red = read(fd, &buffer, BUFSIZ);
    

    while (bytes_red > 0)
    {
        while (buffer[pos] != '\n' && bytes_red > 0)
        {
            bytes_red--;
            str_len++;
            pos++;
        }
        
        if (buffer[pos] == '\n')
        {
            if (allocated < str_cnt + 1)
            {
                allocated *= 2;
                if ((tmp = realloc(matrix, allocated * sizeof(str))) != NULL)
                {
                    matrix = tmp;
                }
                else
                {
                    perror("Realloc error");
                }
            }
            matrix[str_cnt].offset = last + pos - str_len;
            matrix[str_cnt].len = str_len;

            str_len = 0;
            str_cnt++;
            pos++;
            bytes_red--;
        }
        
        if (bytes_red == 0)
        {
            last += pos;
            pos = 0;
            bytes_red = read(fd, &buffer, BUFSIZ);
        }
    }
    
    printf("Choose the line: ");
    scanf("%d", &choice);
    while (choice != 0)
    {
        if (choice > str_cnt)
        {
            printf("No such line. There are %d lines.\n", str_cnt);
        }
        else
        {
            char take_str[(int)matrix[choice - 1].len];
            lseek(fd, matrix[choice - 1].offset, SEEK_SET);
            read(fd, take_str, matrix[choice - 1].len);
            //write(1, take_str, matrix[choice - 1][1]);
            printf("%.*s\n", (int)matrix[choice - 1].len, take_str);
        }

        printf("Choose the line: ");
        scanf("%d", &choice);
    }
    
    free(matrix);
    close(fd);
    exit(0);
}
