#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include "constants.h"
#include "line.h"

// Gets the next line that contains actual tokens.
// Only stores from the first token to the 
// end of the line or the start of a comment.
int get_line(FILE* file, char* line) {
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
        if (ch == EOF) {
            return 0;
        }
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

int get_token(FILE* file, char* token) {
    int ch;
    int i = 0;

    while (((ch = toupper(fgetc(file))) != EOF) && isspace((unsigned char)ch));

    if (ch == EOF) {
        return 0;
    }

    while (ch != EOF && !isspace((unsigned char)ch)) {
        if (i < STRLEN - 1) {
            token[i++] = (unsigned char)ch;
        }
        ch = toupper(fgetc(file));
    }
    token[i] = '\0';

    return 1;
}

void loop(const char* fpath) {
    FILE* file = fopen(fpath, "r");

    //char token[STRLEN];
    //while (get_token(file, token)) {
    //    Operand* operand = (Operand*)malloc(sizeof(Operand));
    //    OperandType parse_success = parse_operand(token, operand);
    char line[STRLEN];
    while (get_line(file, line)) {
        printf("%s\n", line);
    }

    fclose(file);
}
