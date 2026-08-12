#ifndef TOKEN
#define TOKEN

#include <stdint.h>
#include "constants.h"

typedef enum {
    LABEL_VALID,
    LABEL_INVALID
} LabelResult;

LabelResult parse_label(const char* token);

// Operands
typedef enum {
    OPERAND_IVAL,
    OPERAND_LABEL,
    OPERAND_REG,
    OPERAND_STRINGZ,
    OPERAND_INVALID
} OperandType;

typedef enum {
    OPT_STR = 1 << 0,
    OPT_LAB = 1 << 1,
    OPT_IVA = 1 << 2,
    OPT_REG = 1 << 3,
} OperandOption;

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
    int n_ops;
    OperandOption op_types[3];
    int ival_signed;
} OperationSpec;

typedef struct {
    const char* token;
    OperationCode opcode;
    int strncmp;
    OperationSpec opspec;
} OperationCodeMap;

static const OperationCodeMap opcode_map[] = {
    {"ADD",   ADD,   3, {3, {OPT_REG, OPT_REG, OPT_REG|OPT_IVA}, 1}},
    {"AND",   AND,   3, {3, {OPT_REG, OPT_REG, OPT_REG|OPT_IVA}, 1}},
    {"NOT",   NOT,   3, {2, {OPT_REG, OPT_REG, 0},               0}},
    {"LD",    LD,    2, {2, {OPT_REG, OPT_LAB, 0},               1}},
    {"LDI",   LDI,   3, {2, {OPT_REG, OPT_LAB, 0},               1}},
    {"LDR",   LDR,   3, {3, {OPT_REG, OPT_REG, OPT_IVA},         1}},
    {"LEA",   LEA,   3, {2, {OPT_REG, OPT_LAB, 0},               1}},
    {"ST",    ST,    2, {2, {OPT_REG, OPT_LAB, 0},               1}},
    {"STR",   STR,   3, {3, {OPT_REG, OPT_REG, OPT_IVA},         1}},
    {"STI",   STI,   3, {2, {OPT_REG, OPT_LAB, 0},               1}},
    {"BR",    BR,    2, {1, {OPT_LAB, 0, 0},                     1}},
    {"JSR",   JSR,   3, {1, {OPT_LAB, 0, 0},                     1}},
    {"JSRR",  JSRR,  4, {1, {OPT_REG, 0, 0},                     0}},
    {"JMP",   JMP,   3, {1, {OPT_REG, 0, 0},                     0}},
    {"RTI",   RTI,   3, {0, {0, 0, 0},                           0}},
    {"TRAP",  TRAP,  4, {1, {OPT_IVA, 0, 0},                     0}},
    {"GETC",  GETC,  4, {0, {0, 0, 0},                           0}},
    {"OUT",   OUT,   3, {0, {0, 0, 0},                           0}},
    {"PUTS",  PUTS,  4, {0, {0, 0, 0},                           0}},
    {"IN",    IN,    2, {0, {0, 0, 0},                           0}},
    {"PUTSP", PUTSP, 5, {0, {0, 0, 0},                           0}},
    {"HALT",  HALT,  4, {0, {0, 0, 0},                           0}},
};

OperationCode parse_opcode(const char* token);

// Directives
typedef enum { ORIG, END, FILL, BLKW, STRINGZ, DIRECTIVE_INVALID } DirectiveType;

typedef struct {
    const char* token;
    DirectiveType directive;
    OperationSpec opspec;
} DirectiveTypeMap;

static const DirectiveTypeMap directive_map[] = {
    {".ORIG",    ORIG,    {1, {OPT_IVA, 0, 0},        0}},
    {".END",     END,     {0, {0, 0, 0},               0}},
    {".FILL",    FILL,    {1, {OPT_IVA|OPT_LAB, 0, 0}, 1}},
    {".BLKW",    BLKW,    {1, {OPT_IVA, 0, 0},        0}},
    {".STRINGZ", STRINGZ, {1, {OPT_STR, 0, 0},         0}},
};

DirectiveType parse_directivetype(const char* token);

#endif
