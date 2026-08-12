#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "assembler/token.h"
#include "constants.h"

static int _validate_label(const char* token);
static int _is_dec(const char* token);
static int _is_hex(const char* token);
static int _is_register(const char* token, Register* reg);
static int _is_opcode(const char* token, OperationCodeMap* opcodemap);
static int _is_directive(const char* token, DirectiveTypeMap* dirtypemap);
static Operand* _parse_operand_ival(const char* token, int is_dec);
static Operand* _parse_operand_reg(Register reg);
static Operand* _parse_operand_label(const char* token);

LabelResult parse_label(const char* token) {
    if (_validate_label(token)) return LABEL_VALID;

    return LABEL_INVALID;
}

Operand* parse_operand(const char* token) {
    if (_is_dec(token)) {
        return _parse_operand_ival(token, 1);
    }
    else if (_is_hex(token)) {
        return _parse_operand_ival(token, 0);
    }

    for (int i = 0; i < (int)(sizeof(reg_map) / sizeof(reg_map[0])); i++) {
        if (strcmp(token, reg_map[i].token) == 0) {
            return _parse_operand_reg(reg_map[i].reg);
        }
    }

    return _parse_operand_label(token);
}

OperationCodeMap* parse_opcode(const char* token) {
    OperationCodeMap* opcodemap = (OperationCodeMap*)malloc(sizeof(OperationCodeMap));
    _is_opcode(token, opcodemap);

    return opcodemap;
}

DirectiveTypeMap* parse_dirtype(const char* token) {
    DirectiveTypeMap* dirtypemap = (DirectiveTypeMap*)malloc(sizeof(DirectiveTypeMap));
    _is_directive(token, dirtypemap);

    return dirtypemap;
}

static int _validate_label(const char* token) {
    if (_is_hex(token)) return 0;
    if (_is_register(token, NULL)) return 0;
    if (_is_opcode(token, NULL)) return 0;
    if (_is_directive(token, NULL)) return 0;
    if (!(token[0] >= 'A' && token[0] <= 'Z')) return 0;

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

static int _is_opcode(const char* token, OperationCodeMap* opcodemap) {
    int i;

    for (i = 0; i < (int)(sizeof(opcode_map) / sizeof(opcode_map[0])); i++) {
        if (strcmp(token, opcode_map[i].token) == 0) {
            if (opcodemap) *opcodemap = opcode_map[i];
            return 1;
        }
    }

    if (opcodemap) *opcodemap = opcode_map[i-1];
    return 0;
}

static int _is_directive(const char* token, DirectiveTypeMap* dirtypemap) {
    int i;

    for (i = 0; i < (int)(sizeof(directive_map) / sizeof(directive_map[0])); i++) {
        if (strcmp(token, directive_map[i].token) == 0) {
            if (dirtypemap) *dirtypemap = directive_map[i];
            return 1;
        }
    }

    if (dirtypemap) *dirtypemap = directive_map[i-1];
    return 0;
}

static Operand* _parse_operand_ival(const char* token, int is_dec) {
    Operand* operand = (Operand*)malloc(sizeof(Operand));
    operand->type = OPT_IVA;

    if (is_dec) {
        operand->operand.ival = (int16_t)strtol(++token, NULL, 10);
    }
    else {
        token += token[0] == 'X' ? 1 : 2;
        operand->operand.ival = (int16_t)strtol(token, NULL, 16);
    }

    return operand;
}

static Operand* _parse_operand_reg(Register reg) {
    Operand* operand = (Operand*)malloc(sizeof(Operand));

    operand->type = OPT_REG;
    operand->operand.reg = reg;

    return operand;
}

static Operand* _parse_operand_label(const char* token) {
    Operand* operand = (Operand*)malloc(sizeof(Operand));

    operand->type = OPT_LAB;

    if (!_validate_label(token)) {
        operand->type = OPERAND_INVALID;
        return operand;
    }

    strncpy(operand->operand.label, token, STRLEN-1);
    operand->operand.label[STRLEN - 1] = '\0';

    return operand;
}
