#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include "constants.h"
#include "assembler/token.h"
#include "assembler/line.h"

// Gets the next line of a file that contains a token
static int _get_fline(FILE* f, char* line);
static void _skip_comment(FILE* f, int* ch);
static int _get_token(char** line, char* token);
static Operation* _get_operation(const char* token, OperationCodeMap* opcodemap);
static Directive* _get_directive(const char* token, DirectiveTypeMap* dirtypemap);

Line* parse_line(FILE* f) {
    Line* line = (Line*)malloc(sizeof(Line));
    line->err = LINE_ERR_NONE;
    char fline[STRLEN];

    if (!_get_fline(f, fline)) {
        return NULL;
    }

    printf("%s\n", fline);

    char token[STRLEN];
    char* fl = fline;


    // Logic for parsing a line:
    // 1. Check for Label
    // 2. Differentiate between Directive and Operation
    // 3. Get operands and verify token count
    
    // 1 Get first token
    //_get_token(&fl, token);

    //// 2 Check for label
    //if (parse_label(token) == LABEL_VALID) {
    //    strncpy(line->label, token, STRLEN-1);
    //    line->label[STRLEN - 1] = '\0';

    //    // 2.5 Get next token if a label is there
    //    _get_token(&fl, token);
    //}

    //// 3 Differentiate between Directive and Operation
    //OperationCodeMap* opcodemap = parse_opcode(token);
    //DirectiveTypeMap* dirtypemap = parse_directivetype(token);
    //if (opcodemap->opcode != OPCODE_INVALID) {
    //   Operation* op = _get_operation(token, opcodemap);
    //}
    //else if (dirtypemap->dirtype != DIRECTIVE_INVALID) {
    //    Directive* dir = _get_directive(token, dirtypemap);
    //}
    //else {
    //    line->err = LINE_ERR_INVALID_FIRST_TOKEN;
    //    return line;
    //}

    while(_get_token(&fl, token)) {
        LabelResult label = parse_label(token);
        OperationCodeMap* opcodemap = parse_opcode(token);
        DirectiveTypeMap* dirtypemap = parse_dirtype(token);
        Operand* operand = parse_operand(token);

        if (label != LABEL_INVALID) {
            printf("\tLabel: %s\n", token);
        }
        else if (opcodemap->opcode != OPCODE_INVALID) {
            printf("\tOpcode: %s\n", token);
        }
        else if (dirtypemap->dirtype != DIRECTIVE_INVALID) {
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

static Operation* _get_operation(const char* token, OperationCodeMap* opcodemap) {
    Operation* op = (Operation*)malloc(sizeof(Operation));
    op->opcode = opcodemap->opcode;

    return op;
}

static Directive* _get_directive(const char* token, DirectiveTypeMap* dirtypemap) {
    Directive* dir = (Directive*)malloc(sizeof(Directive));
    dir->type = dirtypemap->dirtype;

    return dir;
}
