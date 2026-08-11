#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include "constants.h"
#include "line.h"

// Gets the next line in file that contains actual tokens.
// Only stores from the first token to the end of the line
// or the start of a comment.
int get_fline(FILE* file, char* line) {
    int ch;
    int i = 0;

    while (((ch = toupper(fgetc(file))) != EOF) && (isspace((unsigned char)ch) || ch == ';')) {
        if (ch == ';') {
            while (((ch = toupper(fgetc(file))) != EOF) && ch != '\n');
        }
    }

    if (ch == EOF) {
        return 0;
    }

    while (ch != ';' && ch != '\n') {
        if (i < STRLEN - 1) {
            line[i++] = (unsigned char)ch;
        }

        ch = toupper(fgetc(file));
    }

    if (ch == ';') {
        while (((ch = toupper(fgetc(file))) != EOF) && ch != '\n');
    }

    line[i] = '\0';

    return 1;
}

// Gets next token from a line.
int get_token(char** line, char* token) {
    int i = 0;

    while (**line != '\0') {
        while (!isspace(**line) && **line != ',') {
            token[i++] = *(*line)++;
        }
        if (i > 0) {
            token[i] = '\0';
            return 1;
        }

        (*line)++;
    }

    token[i] = '\0';
    return 0;
}

void loop(const char* fpath) {
    FILE* file = fopen(fpath, "r");

    char line[STRLEN];
    while (get_fline(file, line)) {
        printf("%s\n", line);

        char token[STRLEN];
        char* p = line; // We need a pointer...
        while(get_token(&p, token)) {
            printf("\t%s\n", token);
        };
    }

    fclose(file);
}
