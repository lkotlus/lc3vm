#include <stdio.h>
#include <ctype.h>
#include "../main.h"

char get_token(FILE* file, char* token) {
    int ch;
    int i = 0;

    while (((ch = fgetc(file)) != EOF) && isspace((unsigned char)ch));

    if (ch == EOF) {
        return 0;
    }

    while (ch != EOF && !isspace((unsigned char)ch)) {
        if (i < STRLEN - 1) {
            token[i++] = (unsigned char)ch;
        }
        ch = fgetc(file);
    }
    token[i] = '\0';

    return 1;
}

void loop(char* fpath) {
    FILE* file = fopen(fpath, "r");

    char token[STRLEN];
    while (get_token(file, token)) {
        printf("%s\n", token);
    }

    fclose(file);
}
