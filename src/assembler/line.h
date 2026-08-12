#ifndef LINE
#define LINE

#include <stdio.h>
#include "assembler/token.h"

// Operations
typedef enum {
    OP_ERR_NONE,
    OP_ERR_TOO_MANY_OPERANDS,
    OP_ERR_TOO_FEW_OPERANDS,
    OP_ERR_WRONG_TYPE_OPERANDS
} OperationErr;

typedef struct {
    OperationErr err;
    int n_ops;
    Operand operands[3];
    OperationCode opcode;
} Operation;

// Directives
typedef enum {
    DIR_ERR_NONE,
    DIR_ERR_TOO_MANY_OPERANDS,
    DIR_ERR_TOO_FEW_OPERANDS,
    DIR_ERR_WRONG_TYPE_OPERANDS
} DirectiveErr;

typedef struct {
    DirectiveErr err;
    DirectiveType type;
    int has_operand;
    Operand operand;
} Directive;

// Lines
typedef enum {
    LINE_ERR_NONE,
    LINE_ERR_INVALID_TOKEN,
    LINE_ERR_OP,
    LINE_ERR_DIR,
} LineErr;

typedef enum {
    OPERATION,
    DIRECTIVE
} LineType;

typedef struct {
    LineErr err;
    uint16_t addr;
    char label[STRLEN];
    LineType type;
    union {
        Directive directive;
        Operation operation;
    } line;
} Line;

Line* parse_line(FILE* f);

#endif
