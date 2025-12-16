#include "utils.hpp"

#include <stdio.h>
#include <ctype.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <time.h>

void CleanBuffer() {
    while (getchar() != '\n') { }
}

void SkipSpaces(FILE* stream) {
    while (isspace(getc(stream))) {}
    fseek(stream, -1, SEEK_CUR);    
}

size_t FileSize(FILE* file) {
    struct stat stats = {};
    fstat(fileno(file), &stats);

    return (size_t)stats.st_size;
}

size_t RandInt(size_t max) {
    static int i = 0;

    if (i == 0) {
        srand((unsigned int)time(NULL));
    }

    i++;
    
    return (size_t)rand() % (max + 1);
}