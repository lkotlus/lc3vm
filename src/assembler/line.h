#ifndef LINE
#define LINE

#include <stdio.h>
#include "assembler/token.h"

// Operations
typedef struct {
    Operand operands[3];
    OperationCode opcode;
} Operation;

// Directives
typedef struct {
    DirectiveType type;
    union {
        uint16_t orig_addr;
        Operand fill_value;
        uint16_t blkw_count;
        char stringz[STRLEN];
    } value;
} Directive;

// Lines
typedef enum {
    OPERATION,
    DIRECTIVE
} LineType;

typedef struct {
    uint16_t addr;
    LineType type;
    union {
        Directive directive;
        Operation operation;
    } line;
} Line;

Line* parse_line(FILE* f);

#endif
