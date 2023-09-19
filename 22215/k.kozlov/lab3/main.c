#include "unistd.h"
#include "stdio.h"
#include "errno.h"

// Для исполнения одногруппниками прописать /home/students/22200/k.kozlov/NSU-OS-2023/22215/k.kozlov/lab3/script_for_groupmates

int tryOpen(char* fileName) {
  uid_t uid = getuid(), euid = geteuid();
  printf("UID: %d / EUID: %d\n", uid, euid);
  FILE* fStream = fopen(fileName, "r");
  if (fStream) {
    fclose(fStream);
    printf("File was read successfully\n");
    return 0;
  }
  else
    return errno;
}

void setuidWrapper(uid_t newEUID) {
  printf("Setting EUID to %d...\n", newEUID);
  if (setuid(newEUID) == 0) {
    printf("Success!\n");
  }
  else 
    printf("Error %d\n", errno);
}

int main(int argc, char** argv) {
  char fileName[1025] = "file"; // Добавить принятие имени из командной строки
  int returnCode = tryOpen(fileName);
  while (returnCode != 0)
  {
    printf("File wasn't read (\nError %d\n", returnCode);
    setuidWrapper(getuid());
    returnCode = tryOpen(fileName);
  }
}