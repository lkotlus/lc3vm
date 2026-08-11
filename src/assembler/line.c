#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include "constants.h"
#include "assembler/token.h"
#include "assembler/line.h"

// Gets the next line of a file that contains a token
static int _get_fline(FILE* f, char* line);
static void _skip_comment(FILE* f, int* ch);
static int _get_token(char** line, char* token);

Line* parse_line(FILE* f) {
    Line* line = (Line*)malloc(sizeof(Line));
    char fline[STRLEN];

    if (!_get_fline(f, fline)) {
        return NULL;
    }

    printf("%s\n", fline);

    char token[STRLEN];
    char* p = fline;
    while(_get_token(&p, token)) {
        LabelResults label = parse_label(token);
        OperationCode opcode = parse_opcode(token);
        DirectiveType directivetype = parse_directivetype(token);
        Operand* operand = parse_operand(token);

        if (label != LABEL_INVALID) {
            printf("\tLabel: %s\n", token);
        }
        else if (opcode != OPCODE_INVALID) {
            printf("\tOpcode: %s\n", token);
        }
        else if (directivetype != DIRECTIVE_INVALID) {
            printf("\tDirective: %s\n", token);
        }
        else if (operand->type != OPERAND_INVALID) {
            printf("\tOperand: %s\n", token);
        } 
    }

    return line;
}

static int _get_fline(FILE* f, char* fline) {
    int ch;
    int i = 0;

    while (((ch = toupper(fgetc(f))) != EOF) && (isspace((unsigned char)ch) || ch == ';')) {
        _skip_comment(f, &ch);
    }

    if (ch == EOF) {
        return 0;
    }

    while (ch != ';' && ch != '\n') {
        if (i < STRLEN - 1) {
            fline[i++] = (unsigned char)ch;
        }

        ch = toupper(fgetc(f));
    }
    _skip_comment(f, &ch);

    fline[i] = '\0';

    return 1;
}

static void _skip_comment(FILE* f, int* ch) {
    if (*ch == ';') {
        while (((*ch = toupper(fgetc(f))) != EOF) && *ch != '\n');
    }
}

static int _get_token(char** fline, char* token) {
    int i = 0;

    while (**fline != '\0') {
        while (!isspace(**fline) && **fline != ',' && **fline != '\0') {
            token[i++] = *(*fline)++;
        }
        if (i > 0) {
            token[i] = '\0';
            return 1;
        }

        (*fline)++;
    }

    token[i] = '\0';
    return 0;
}
