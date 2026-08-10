#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include "constants.h"
#include "line.h"

char get_token(FILE* file, char* token) {
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

char get_line(FILE* file);

void loop(const char* fpath) {
    FILE* file = fopen(fpath, "r");

    char token[STRLEN];
    while (get_token(file, token)) {
        Operand* operand = (Operand*)malloc(sizeof(Operand));
        OperandType parse_success = parse_operand(token, operand);
        printf("%d, %d, %s\n", parse_success, operand->type, token);
    }

    fclose(file);
}
