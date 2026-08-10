#ifndef OPERAND_H
#define OPERAND_H

#include <stdint.h>
#include "main.h"

// Operands
typedef enum {
    OPERAND_IVAL,
    OPERAND_ADDR,
    OPERAND_LABEL,
    OPERAND_REG
} OperandType;
typedef struct {
    OperandType type;
    union {
        int16_t ival;
        uint16_t addr;
        char label[STRLEN];
        uint8_t reg;
    } operand;
} Operand;

// Operations
//
// Notes:
//  1. RET needs to be treated as JMP R7
//  2. BR instructions (BRz, BRnz, etc.) need a bitmap as an ival into operands[1]
typedef enum {
    ADD, AND, NOT, LD,
    LDI, LDR, LEA, ST,
    STR, STI, BR, JSR,
    JSRR, JMP, RTI, TRAP
} OperationCode;
typedef struct {
    Operand operands[3];
    OperationCode opcode;
} Operation;

// Directives
typedef enum { ORIG, END, FILL, BLKW, STRINGZ } DirectiveType;
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

#endif
