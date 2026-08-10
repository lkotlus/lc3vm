#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "line.h"
#include "constants.h"

OperandType _parse_operand_ival(const char* token, Operand* operand);
OperandType _parse_operand_reg(Register reg, Operand* operand);
OperandType _parse_operand_label(const char* token, Operand* operand);

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


OperandType _parse_operand_ival(const char* token, Operand* operand) {
    operand->type = IVAL_OPERAND;

    if (token[0] == '#') {
        // Special case for negative numbers
        if (!((token[1] > 47 && token[1] < 58) || token[1] == '-')) {
            return INVALID_OPERAND;
        }

        int i = 2;
        char c = token[i];

        while (c != '\0') {
            // Valid decimal digits [0-9]
            if (!(c > 47 && c < 58)) {
                return INVALID_OPERAND;
            }
            c = token[++i];
        }

        operand->operand.ival = (int16_t)strtol(++token, NULL, 10);
    }
    else if (token[0] == '0' && token[1] == 'X') {
        int i = 2;
        char c = token[i];

        while (c != '\0') {
            // Valid hex digits [0-9A-F]
            if (!((c > 47 && c < 58) || (c > 64 && c < 71))) {
                return INVALID_OPERAND;
            }
            c = token[++i];
        }
        operand->operand.ival = (int16_t)strtol(++token, NULL, 16);
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

    // The first character must be [A-Z]
    if (!(token[0] > 64 && token[0] < 91)) {
        return INVALID_OPERAND;
    }

    int i = 1;
    char c = token[i];
    while (c != '\0') {
        // All other characters must be [A-Z0-9]
        if (!((c > 47 && c < 58) || (c > 64 && c < 91))) {
            return INVALID_OPERAND;
        }
        c = token[++i];
    }

    strncpy(operand->operand.label, token, STRLEN);

    return LABEL_OPERAND;
}
