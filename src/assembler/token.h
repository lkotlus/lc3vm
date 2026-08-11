#ifndef TOKEN
#define TOKEN

#include <stdint.h>
#include "constants.h"

typedef enum {
    LABEL_VALID,
    LABEL_INVALID
} LabelResults;

LabelResults parse_label(const char* token);

// Operands
typedef enum {
    OPERAND_IVAL,
    OPERAND_LABEL,
    OPERAND_REG,
    OPERAND_INVALID
} OperandType;

typedef enum {
    R0, R1, R2, R3,
    R4, R5, R6, R7
} Register;

typedef struct {
    OperandType type;
    union {
        int16_t ival;
        Register reg;
        char label[STRLEN];
    } operand;
} Operand;

typedef struct {
    const char* token;
    Register reg;
} RegisterMap;

static const RegisterMap reg_map[] = {
    {"R0", R0}, {"R1", R1}, {"R2", R2}, {"R3", R3},
    {"R4", R4}, {"R5", R5}, {"R6", R6}, {"R7", R7}
};

Operand* parse_operand(const char* token);

// Operations
//
// Notes:
//  1. RET needs to be treated as JMP R7
//  2. BR instructions (BRz, BRnz, etc.) need a bitmap as an ival into operands[1]
typedef enum {
    ADD, AND, NOT, LD,
    LDI, LDR, LEA, ST,
    STR, STI, BR, JSR,
    JSRR, JMP, RTI, TRAP,
    GETC, OUT, PUTS, IN,
    PUTSP, HALT, OPCODE_INVALID
} OperationCode;

typedef struct {
    const char* token;
    OperationCode opcode;
    int n;
} OperationCodeMap;

static const OperationCodeMap opcode_map[] = {
    {"ADD", ADD, 3}, {"AND", AND, 3}, {"NOT", NOT, 3}, {"LD", LD, 2},
    {"LDI", LDI, 3}, {"LDR", LDR, 3}, {"LEA", LEA, 3}, {"ST", ST, 2},
    {"STR", STR, 3}, {"STI", STI, 3}, {"BR", BR, 2}, {"JSR", JSR, 3},
    {"JSRR", JSRR, 4}, {"JMP", JMP, 3}, {"RTI", RTI, 3}, {"TRAP", TRAP, 4},
    {"GETC", GETC, 4}, {"OUT", OUT, 3}, {"PUTS", PUTS, 4}, {"IN", IN, 2},
    {"PUTSP", PUTSP, 5}, {"HALT", HALT, 4}
};

OperationCode parse_opcode(const char* token);

// Directives
typedef enum { ORIG, END, FILL, BLKW, STRINGZ, DIRECTIVE_INVALID } DirectiveType;

typedef struct {
    const char* token;
    DirectiveType directive;
} DirectiveMap;

static const DirectiveMap directive_map[] = {
    {".ORIG", ORIG}, 
    {".END", END}, 
    {".FILL", FILL}, 
    {".BLKW", BLKW},
    {".STRINGZ", STRINGZ}
};

DirectiveType parse_directivetype(const char* token);

#endif
