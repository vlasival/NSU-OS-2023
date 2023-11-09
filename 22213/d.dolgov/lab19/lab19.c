#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <regex.h>
#include <errno.h>

#define MAX_PATTERN_LENGTH 100

struct dirent *file;
regex_t regex;

int letterCounter = 0;
int flag = 0;

enum argCheckerStatuses {
    SUCCESS, TOO_MANY_ARGUMENTS, NO_ARGUMENTS
};

enum stringCheckerStatuses {
    STRING_CHECKER_SUCCESS, SLASH_FOUND
};

int argCheck(int argLength) {
    if (argLength == 2) {
        return SUCCESS;
    }

    if (argLength == 1) {
        return NO_ARGUMENTS;
    }

    return TOO_MANY_ARGUMENTS;
}

int checkString(char *searchString) {
    if (strstr(searchString, "/") != NULL) {
        return SLASH_FOUND;
    }

    return STRING_CHECKER_SUCCESS;
}

void parseLetter(const char *pattern, char *newPattern, int i, int *counter) {
    if (pattern[i] == '?') {
        newPattern[*counter] = '\\';
        (*counter)++;
        newPattern[*counter] = 'w';
        (*counter)++;
        letterCounter++;
    } else if (pattern[i] == '*') {
        newPattern[*counter] = '.';
        (*counter)++;
        newPattern[*counter] = '*';
        (*counter)++;
        flag++;
    } else if (pattern[i] == '.') {
        newPattern[*counter] = '\\';
        (*counter)++;
        newPattern[*counter] = '.';
        (*counter)++;
    } else {
        newPattern[*counter] = pattern[i];
        (*counter)++;
    }
}

void buildRegexp(char *pattern, int patternLength) {
    char *newPattern = malloc(sizeof(char) * MAX_PATTERN_LENGTH);

    if (newPattern == NULL) {
        perror("COULD NOT ALLOCATE MEMORY FOR REGEXP\n");
    }

    int counter = 0;

    for (int i = 0; i < patternLength; i++) {
        parseLetter(pattern, newPattern, i, &counter);
    }

    int reti = regcomp(&regex, newPattern, 0);

    if (reti) {
        perror("COULD NOT COMPILE REGEX\n");
        exit(1);
    }

    free(newPattern);
}

void readFiles(DIR *folder, int *notFoundFlag) {

    while ((file = readdir(folder))) {
        errno = 0;

        char *fileName = malloc(sizeof(char) * (strlen(file->d_name) + 1));

        if (fileName == NULL) {
            perror("COULD NOT ALLOCATE MEMORY FOR FILENAME\n");
        }

        strcpy(fileName, file->d_name);

        if (!regexec(&regex, fileName, 0, NULL, 0)) {
            printf("%s ", file->d_name);
            (*notFoundFlag)++;
        } else if (flag == 0 && strlen(fileName) > letterCounter) {
            (*notFoundFlag)++;
        }

        free(fileName);
    }

    if (errno != 0) {
        perror("ERROR READING DIRECTORY\n");
    }

    closedir(folder);
}

void findFiles(char *pattern, int patternLength) {
    DIR *folder = opendir(".");
    int notFoundFlag = 0;

    if (folder == NULL) {
        perror("UNABLE TO READ THE DIRECTORY\n");
        exit(1);
    }

    buildRegexp(pattern, patternLength);
    readFiles(folder, &notFoundFlag);

    if (notFoundFlag == 0) {
        printf("%s", pattern);
    }

    printf("\n");
}

void propagate(char *rawPattern) {
    int patternLength = (int) strlen(rawPattern);
    char *pattern = malloc(sizeof(char) * patternLength);

    if (pattern == NULL) {
        perror("COULD NOT ALLOCATE MEMORY FOR PATTERN\n");
    }

    strcpy(pattern, rawPattern);

    if (checkString(pattern) == SLASH_FOUND) {
        perror("NO SLASHES ALLOWED IN THE SEARCH PATTERN\n");
    } else {
        findFiles(pattern, patternLength);
    }

    free(pattern);
}

char *patternInput() {
    printf("Please enter your file pattern\n");
    char *pattern = malloc(sizeof(char) * MAX_PATTERN_LENGTH);

    if (pattern == NULL) {
        perror("COULD NOT ALLOCATE MEMORY FOR PATTERN\n");
    }

    scanf("%s", pattern);

    return pattern;
}

int main(int argc, char *argv[]) {
    char *pattern = patternInput();

    propagate(pattern);

    free(pattern);
    exit(0);
}

