#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <regex.h>
#include <errno.h>

#define MAX_PATTERN_LENGTH 100

struct dirent *file;
regex_t regex;

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
        newPattern[*counter] = '.';
        (*counter)++;
    } else if (pattern[i] == '*') {
        newPattern[*counter] = '.';
        (*counter)++;
        newPattern[*counter] = '*';
        (*counter)++;
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
    int counter = 0;
    for (int i = 0; i < patternLength; i++) {
        parseLetter(pattern, newPattern, i, &counter);
    }

    int reti = regcomp(&regex, newPattern, 0);

    if (reti) {
        fprintf(stderr, "COULD NOT COMPILE REGEX\n");
        exit(1);
    }

    free(newPattern);
}

void readFiles(DIR *folder, int *notFoundFlag) {

    while ((file = readdir(folder))) {
        errno = 0;

        char *fileName = malloc(sizeof(char) * (strlen(file->d_name) + 1));
        strcpy(fileName, file->d_name);

        if (!regexec(&regex, fileName, 0, NULL, 0)) {
            printf("%s ", file->d_name);
            (*notFoundFlag)++;
        }

        free(fileName);
    }

    if (errno != 0) {
        fprintf(stderr, "ERROR READING DIRECTORY\n");
    }

    closedir(folder);
}

void findFiles(char *pattern, int patternLength) {
    DIR *folder = opendir(".");
    int notFoundFlag = 0;

    if (folder == NULL) {
        fprintf(stderr, "UNABLE TO READ THE DIRECTORY\n");
        exit(1);
    }

    buildRegexp(pattern, patternLength);
    readFiles(folder, &notFoundFlag);

    if (notFoundFlag == 0) {
        printf("%s", pattern);
    }

    printf("\n");
}

void propagate(char *argv[]) {
    int patternLength = (int) strlen(argv[1]);
    char *pattern = malloc(sizeof(char) * patternLength);
    strcpy(pattern, argv[1]);

    if (checkString(pattern) == SLASH_FOUND) {
        fprintf(stderr, "NO SLASHES ALLOWED IN THE SEARCH PATTERN %s\n", pattern);
    } else {
        findFiles(pattern, patternLength);
    }

    free(pattern);
}

int main(int argc, char *argv[], char *envp[]) {
    int checkResult = argCheck(argc);
    if (checkResult == SUCCESS) {
        propagate(argv);
    } else if (checkResult == NO_ARGUMENTS) {
        fprintf(stderr, "NO ARGUMENTS\n");
    } else {
        fprintf(stderr, "TOO MANY ARGUMENTS\n");
    }

    exit(0);
}

