#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "line.h"
#include "constants.h"

int _validate_label(const char* token);
int _is_hex(const char* token);
int _is_register(const char* token, Register* reg);
int _is_opcode(const char* token, OperationCode* opcode);
int _is_directive(const char* token, DirectiveType* directive);
OperandType _parse_operand_ival(const char* token, Operand* operand);
OperandType _parse_operand_reg(Register reg, Operand* operand);
OperandType _parse_operand_label(const char* token, Operand* operand);

// Returns 1 if you have a register.
// Returns 2 if you have an opcode.
// Returns 3 if you have a directive.
// Returns -1 if you have an invalid label name.
// Returns 0 if you have a label.
int parse_label(const char* token) {
    if (_is_register(token, NULL)) return 1;
    if (_is_opcode(token, NULL)) return 2;
    if (_is_directive(token, NULL)) return 3;
    if (!_validate_label(token)) return -1;

    return 0;
}

OperandType parse_operand(const char* token, Operand* operand) {
    if (token[0] == '#' || token[0] == '0') {
        return _parse_operand_ival(token, operand);
    }

    for (int i = 0; i < (int)(sizeof(reg_map) / sizeof(reg_map[0])); i++) {
        if (strcmp(token, reg_map[i].token) == 0) {
            return _parse_operand_reg(reg_map[i].reg, operand);
        }
    }

    return _parse_operand_label(token, operand);
}

OperationCode parse_operation(const char* token, Operation* operation) {
    return INVALID_OPCODE;
}

DirectiveType parse_directive(const char* token, Directive* directive) {
    return INVALID_DIRECTIVE;
}


int _validate_label(const char* token) {
    if (_is_hex(token)) {
        return 0;
    }

    // The first character must be [A-Z]
    if (!(token[0] > 64 && token[0] < 91)) {
        return 0;
    }

    int i = 1;
    char c = token[i];
    while (c != '\0') {
        // All other characters must be [A-Z0-9]
        if (!((c > 47 && c < 58) || (c > 64 && c < 91))) {
            return 0;
        }
        c = token[++i];
    }

    return 1;
}

int _is_dec(const char* token) {
    if (token[0] == '#') {
        int i = 2;
        char c = token[i];

        // Special case for negative numbers
        if (!((token[1] > 47 && token[1] < 58) || token[1] == '-')) {
            return 0;
        }

        while (c != '\0') {
            // Valid decimal digits [0-9]
            if (!(c > 47 && c < 58)) {
                return 0;
            }
            c = token[++i];
        }

        return 1;
    }

    return 0;
}

int _is_hex(const char* token) {
    if (token[0] == 'X' || (token[0] == '0' && token[1] == 'X')) {
        int i = token[0] == 'X' ? 1 : 2;
        char c = token[i];

        while (c != '\0') {
            // Valid hex digits [0-9A-F]
            if (!((c > 47 && c < 58) || (c > 64 && c < 71))) {
                return 0;
            }
            c = token[++i];
        }

        return 1;
    }

    return 0;
}

int _is_register(const char* token, Register* reg) {
    for (int i = 0; i < (int)(sizeof(reg_map) / sizeof(reg_map[0])); i++) {
        if (strcmp(token, reg_map[i].token) == 0) {
            if (reg) *reg = reg_map[i].reg;
            return 1;
        }
    }

    return 0;
}

int _is_opcode(const char* token, OperationCode* opcode) {
    for (int i = 0; i < (int)(sizeof(opcode_map) / sizeof(opcode_map[0])); i++) {
        if (strcmp(token, opcode_map[i].token) == 0) {
            if (opcode) *opcode = opcode_map[i].opcode;
            return 1;
        }
    }

    return 0;
}

int _is_directive(const char* token, DirectiveType* directive) {
    for (int i = 0; i < (int)(sizeof(directive_map) / sizeof(directive_map[0])); i++) {
        if (strcmp(token, directive_map[i].token) == 0) {
            if (directive) *directive = directive_map[i].directive;
            return 1;
        }
    }

    return 0;
}

OperandType _parse_operand_ival(const char* token, Operand* operand) {
    operand->type = IVAL_OPERAND;

    if (_is_dec(token)) {
        operand->operand.ival = (int16_t)strtol(++token, NULL, 10);
    }
    else if (_is_hex(token)) {
        token += token[0] == 'X' ? 1 : 2;
        operand->operand.ival = (int16_t)strtol(token, NULL, 16);
    }

    return IVAL_OPERAND;
}

OperandType _parse_operand_reg(Register reg, Operand* operand) {
    operand->type = REG_OPERAND;
    operand->operand.reg = reg;

    return REG_OPERAND;
}

OperandType _parse_operand_label(const char* token, Operand* operand) {
    operand->type = LABEL_OPERAND;

    if (!_validate_label(token)) {
        return INVALID_OPERAND;
    }

    strncpy(operand->operand.label, token, STRLEN);

    return LABEL_OPERAND;
}
