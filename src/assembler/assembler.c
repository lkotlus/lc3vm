#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include "constants.h"
#include "assembler/token.h"
#include "assembler/line.h"

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

int parse_token(char* token) {
    return 0;
}

// Gets the next line in file that contains actual tokens.
// Only stores from the first token to the end of the line
// or the start of a comment.
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

int parse_line(FILE* file, Line* line) {
    int ntok = 0;
    char linestr[STRLEN];

    if (get_line(file, linestr)) {
        char token[STRLEN];
        char* p = linestr;
        while (get_token(&p, token)) {
            ntok++;
        };

        return ntok;
    }
    return 0;
}

void loop(const char* fpath) {
    FILE* file = fopen(fpath, "r");

    char line[STRLEN];
    Operand* o = (Operand*)malloc(sizeof(Operand));
    while (get_line(file, line)) {
        printf("%s\n", line);

        char token[STRLEN];
        char* p = line; // We need a pointer...
        while(get_token(&p, token)) {
            if (parse_label(token) == 0) {
                printf("\tLabel: %s\n", token);
            }
            else if (parse_opcode(token) != INVALID_OPCODE) {
                printf("\tOpcode: %s\n", token);
            }
            else if (parse_directivetype(token) != INVALID_DIRECTIVE) {
                printf("\tDirective: %s\n", token);
            }
            else if (parse_operand(token, o) != INVALID_OPERAND) {
                printf("\tOperand: %s\n", token);
            } 
        };
    }

    fclose(file);
}
