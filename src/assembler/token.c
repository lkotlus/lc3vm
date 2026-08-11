#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "assembler/token.h"
#include "constants.h"

static int _validate_label(const char* token);
static int _is_dec(const char* token);
static int _is_hex(const char* token);
static int _is_register(const char* token, Register* reg);
static int _is_opcode(const char* token, OperationCode* opcode);
static int _is_directive(const char* token, DirectiveType* directive);
static OperandType _parse_operand_ival(const char* token, int is_dec, Operand* operand);
static OperandType _parse_operand_reg(Register reg, Operand* operand);
static OperandType _parse_operand_label(const char* token, Operand* operand);

LabelResults parse_label(const char* token) {
    if (_is_register(token, NULL)) return LABEL_INVALID_REGISTER;
    if (_is_opcode(token, NULL)) return LABEL_INVALID_OPCODE;
    if (_is_directive(token, NULL)) return LABEL_INVALID_DIRECTIVE;
    if (!_validate_label(token)) return LABEL_INVALID;

    return LABEL_VALID;
}

OperandType parse_operand(const char* token, Operand* operand) {
    if (_is_dec(token)) {
        return _parse_operand_ival(token, 1, operand);
    }
    else if (_is_hex(token)) {
        return _parse_operand_ival(token, 0, operand);
    }

    for (int i = 0; i < (int)(sizeof(reg_map) / sizeof(reg_map[0])); i++) {
        if (strcmp(token, reg_map[i].token) == 0) {
            return _parse_operand_reg(reg_map[i].reg, operand);
        }
    }

    return _parse_operand_label(token, operand);
}

OperationCode parse_opcode(const char* token) {
    OperationCode opcode;
    if (_is_opcode(token, &opcode)) {
        return opcode;
    }

    return INVALID_OPCODE;
}

DirectiveType parse_directivetype(const char* token) {
    DirectiveType directive;
    if (_is_directive(token, &directive)) {
        return directive;
    }

    return INVALID_DIRECTIVE;
}

static int _validate_label(const char* token) {
    if (_is_hex(token)) {
        return 0;
    }

    // The first character must be [A-Z]
    if (!(token[0] >= 'A' && token[0] <= 'Z')) {
        return 0;
    }

    int i = 1;
    char c = token[i];
    while (c != '\0') {
        if (!((c >= '0' && c <= '9') || (c >= 'A' && c <= 'Z'))) {
            return 0;
        }
        c = token[++i];
    }

    return 1;
}

static int _is_dec(const char* token) {
    if (token[0] == '#') {
        int i = 2;
        char c = token[i];

        // Special case for negative numbers
        if (!((token[1] >= '0' && token[1] <= '9') || ((token[1] == '-') && token[2] != '\0'))) {
            return 0;
        }

        while (c != '\0') {
            // Valid decimal digits [0-9]
            if (!(c >= '0' && c <= '9')) {
                return 0;
            }
            c = token[++i];
        }

        return 1;
    }

    return 0;
}

static int _is_hex(const char* token) {
    if (token[0] == 'X' || (token[0] == '0' && token[1] == 'X')) {
        int i = token[0] == 'X' ? 1 : 2;
        char c = token[i];

        while (c != '\0') {
            // Valid hex digits [0-9A-F]
            if (!((c >= '0' && c <= '9') || (c >= 'A' && c <= 'F'))) {
                return 0;
            }
            c = token[++i];
        }

        return 1;
    }

    return 0;
}

static int _is_register(const char* token, Register* reg) {
    for (int i = 0; i < (int)(sizeof(reg_map) / sizeof(reg_map[0])); i++) {
        if (strcmp(token, reg_map[i].token) == 0) {
            if (reg) *reg = reg_map[i].reg;
            return 1;
        }
    }

    return 0;
}

static int _is_opcode(const char* token, OperationCode* opcode) {
    for (int i = 0; i < (int)(sizeof(opcode_map) / sizeof(opcode_map[0])); i++) {
        if (strcmp(token, opcode_map[i].token) == 0) {
            if (opcode) *opcode = opcode_map[i].opcode;
            return 1;
        }
    }

    return 0;
}

static int _is_directive(const char* token, DirectiveType* directive) {
    for (int i = 0; i < (int)(sizeof(directive_map) / sizeof(directive_map[0])); i++) {
        if (strcmp(token, directive_map[i].token) == 0) {
            if (directive) *directive = directive_map[i].directive;
            return 1;
        }
    }

    return 0;
}

static OperandType _parse_operand_ival(const char* token, int is_dec, Operand* operand) {
    operand->type = IVAL_OPERAND;

    if (is_dec) {
        operand->operand.ival = (int16_t)strtol(++token, NULL, 10);
    }
    else {
        token += token[0] == 'X' ? 1 : 2;
        operand->operand.ival = (int16_t)strtol(token, NULL, 16);
    }

    return IVAL_OPERAND;
}

static OperandType _parse_operand_reg(Register reg, Operand* operand) {
    operand->type = REG_OPERAND;
    operand->operand.reg = reg;

    return REG_OPERAND;
}

static OperandType _parse_operand_label(const char* token, Operand* operand) {
    operand->type = LABEL_OPERAND;

    if (!_validate_label(token)) {
        return INVALID_OPERAND;
    }

    strncpy(operand->operand.label, token, STRLEN-1);
    operand->operand.label[STRLEN - 1] = '\0';

    return LABEL_OPERAND;
}
