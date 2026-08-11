#ifndef TOKEN
#define TOKEN

#include <stdint.h>
#include "constants.h"

typedef enum {
    LABEL_VALID,
    LABEL_INVALID,
    LABEL_INVALID_REGISTER,
    LABEL_INVALID_OPCODE,
    LABEL_INVALID_DIRECTIVE
} LabelResults;

LabelResults parse_label(const char* token);

// Operands
typedef enum {
    IVAL_OPERAND,
    LABEL_OPERAND,
    REG_OPERAND,
    INVALID_OPERAND
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

OperandType parse_operand(const char* token, Operand* operand);

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
    PUTSP, HALT, INVALID_OPCODE
} OperationCode;

typedef struct {
    const char* token;
    OperationCode opcode;
} OperationCodeMap;

static const OperationCodeMap opcode_map[] = {
    {"ADD", ADD}, {"AND", AND}, {"NOT", NOT}, {"LD", LD},
    {"LDI", LDI}, {"LDR", LDR}, {"LEA", LEA}, {"ST", ST},
    {"STR", STR}, {"STI", STI}, {"BR", BR}, {"JSR", JSR},
    {"JSRR", JSRR}, {"JMP", JMP}, {"RTI", RTI}, {"TRAP", TRAP},
    {"GETC", GETC}, {"OUT", OUT}, {"PUTS", PUTS}, {"IN", IN},
    {"PUTSP", PUTSP}, {"HALT", HALT}
};

OperationCode parse_opcode(const char* token);

// Directives
typedef enum { ORIG, END, FILL, BLKW, STRINGZ, INVALID_DIRECTIVE } DirectiveType;

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
