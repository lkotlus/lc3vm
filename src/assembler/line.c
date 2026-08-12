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
static Operation* _get_operation(char** fl, OperationCodeMap* opcodemap);
static Directive* _get_directive(char** fl, DirectiveTypeMap* dirtypemap);

Line* parse_line(FILE* f) {
    Line* line = (Line*)malloc(sizeof(Line));
    line->err = LINE_ERR_NONE;
    line->label[0] = '\0';
    char fline[STRLEN];

    if (!_get_fline(f, fline)) {
        return NULL;
    }

    char token[STRLEN];
    char* fl = fline;

    printf("%s\n", fline);

    _get_token(&fl, token);

    if (parse_label(token) == LABEL_VALID) {
        strncpy(line->label, token, STRLEN-1);
        line->label[STRLEN - 1] = '\0';

        _get_token(&fl, token);
    }

    OperationCodeMap* opcodemap = parse_opcode(token);
    DirectiveTypeMap* dirtypemap = parse_dirtype(token);
    if (opcodemap->opcode != OPCODE_INVALID) {
       Operation* op = _get_operation(&fl, opcodemap);
       line->type = LINE_OPERATION;
       line->line.operation = *op;

       if (op->err != OP_ERR_NONE) line->err = LINE_ERR_OP;
    }
    else if (dirtypemap->dirtype != DIRECTIVE_INVALID) {
        Directive* dir = _get_directive(&fl, dirtypemap);
        line->type = LINE_DIRECTIVE;
        line->line.directive = *dir;

        if (dir->err != DIR_ERR_NONE) line->err = LINE_ERR_DIR;
    }
    else {
        line->err = LINE_ERR_INVALID_TOKEN;
        return line;
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

static Operation* _get_operation(char** fl, OperationCodeMap* opcodemap) {
    char token[STRLEN];
    Operation* op = (Operation*)malloc(sizeof(Operation));
    op->err = OP_ERR_NONE;
    op->opcode = opcodemap->opcode;
    op->n_ops = opcodemap->opspec.n_ops;

    for (int i = 0; i < opcodemap->opspec.n_ops; i++) {
        Operand* operand;
        
        if (!_get_token(fl, token)) {
            op->err = OP_ERR_TOO_FEW_OPERANDS;
            return op;
        }
        
        operand = parse_operand(token);

        // Some serious voodoo going on here.
        if (!(operand->type & opcodemap->opspec.op_types[i])) {
            op->err = OP_ERR_WRONG_TYPE_OPERANDS;
            return op;
        }

        op->operands[i] = *operand;
    }

    if (_get_token(fl, token)) op->err = OP_ERR_TOO_MANY_OPERANDS;

    return op;
}

static Directive* _get_directive(char** fl, DirectiveTypeMap* dirtypemap) {
    char token[STRLEN];
    Directive* dir = (Directive*)malloc(sizeof(Directive));
    dir->err = DIR_ERR_NONE;
    dir->type = dirtypemap->dirtype;
    dir->has_operand = dirtypemap->operand;

    if (dirtypemap->operand) {
        Operand* operand;
        
        if (!_get_token(fl, token)) {
            dir->err = DIR_ERR_TOO_FEW_OPERANDS;
            return dir;
        }
        
        operand = parse_operand(token);

        // Some serious voodoo going on here.
        if (!(operand->type & dirtypemap->operand)) {
            dir->err = DIR_ERR_WRONG_TYPE_OPERANDS;
            return dir;
        }

        dir->operand = *operand;
    }

    return dir;
}
