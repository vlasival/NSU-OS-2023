#include "unistd.h"
#include "stdio.h"
#include "errno.h"

// Для исполнения одногруппниками прописать /home/students/22200/k.kozlov/NSU-OS-2023/22215/k.kozlov/lab3/script_for_groupmates

/**
 * Файл - объект, который можно читать и писать
 * ЛИБО
 * Файл - последовательность байт с адресацией с точностью до байта
 * 
 * fopen(3)(char* path, char* mode) - открывает файл по заданному пути в заданном режиме и возвращает указатель типа FILE*
 *   В случае ошибки возвращает NULL и записывает код ошибки в errno
 *   Является обёрткой над open(2):
 *     - Открывает дескриптор файла по указанному пути. 
 *       Дескриптор - маленькое цело число, относящееся к созданному описанию файла file_t.
 *         В описание входит адрес vnode, сдвиг (offset) и флаги, отвечающие за режим доступа к файлу и другие опции
 *     - Может открыть файлы устройств
 * 
 * 
 * fclose(3)(FILE* stream) - закрывает файл по указателю FILE*. Возвращает 0 в случае успеха
 *   Являет обёрткой над close(2):
 *     - Закрывает дескриптор, указывающий на информацию о файле file_t
 *       Если это последний дескриптор на file_t, то эта структура также удаляется.
 *       Если удалена последняя структура file_t, то будет удалена структура vnode
 * 
*/
void tryOpen(char* fileName) {
  uid_t uid = getuid(), euid = geteuid();
  printf("Reading file \"%s\"...\n", fileName);
  printf("UID: %d / EUID: %d\n", uid, euid);
  FILE* fStream = fopen(fileName, "r");
  if (fStream) {
    printf("File was read successfully\n");
    if (fclose(fStream) != 0)
      perror("Failed to close file");
    else
      printf("File was closed\n");
  }
  else
    perror("Failed to read file");
}

/*
Реальный ID - число, которым и является пользователь в Unix
Эффективный ID - число, которое определяет права доступа. 
По умолчанию равняется реальному.

setuid бит в правах доступа файла позволяет запускать его с эффективным ID владельца файла.
setuid(2) - позволяет не root-пользователю лишь установить свой реальный ID либо, в некоторых системах, опустить свои привилегии.
  root пользователь может выставлять себе любой эффективный euid
*/

void setuidWrapper(uid_t newEUID) {
  printf("Setting EUID to %d...\n", newEUID);
  if (setuid(newEUID) == 0)
    printf("Success!\n");
  else 
    perror("Failed to set EUID");
}

int main(int argc, char** argv) {
  char fileName[5] = "file";
  tryOpen(fileName);
  printf("\n");
  setuidWrapper(getuid());
  printf("\n");
  tryOpen(fileName);
}