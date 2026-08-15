#include "assembler/token.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "constants.h"

static int _validate_label(const char *token);
static int _is_dec(const char *token);
static int _is_hex(const char *token);
static int _is_register(const char *token, Register *reg);
static int _is_opcode(const char *token, OperationCodeMap *opcodemap);
static int _is_directive(const char *token, DirectiveTypeMap *dirtypemap);
static Operand *_parse_operand_imm(const char *token, int is_dec);
static Operand *_parse_operand_reg(Register reg);
static Operand *_parse_operand_label(const char *token);
static Operand *_parse_operand_stringz(const char *token);

LabelResult parse_label(const char *token, LabelList *labell) {
  LabelMap *current = labell->head;

  while (current) {
    if (strncmp(current->label, token, STRLEN) == 0)
      return LABEL_INVALID_DUPLICATE;
    current = current->next;
  }

  if (_validate_label(token)) return LABEL_VALID;

  return LABEL_INVALID_CHARS;
}

Operand *parse_operand(const char *token) {
  if (_is_dec(token)) {
    return _parse_operand_imm(token, 1);
  } else if (_is_hex(token)) {
    return _parse_operand_imm(token, 0);
  } else if (token[0] == '"') {
    return _parse_operand_stringz(token);
  }

  for (int i = 0; i < (int)(sizeof(reg_map) / sizeof(reg_map[0])); ++i) {
    if (strcmp(token, reg_map[i].token) == 0) {
      return _parse_operand_reg(reg_map[i].reg);
    }
  }

  return _parse_operand_label(token);
}

OperationCodeMap *parse_opcode(const char *token) {
  OperationCodeMap *opcodemap =
      (OperationCodeMap *)malloc(sizeof(OperationCodeMap));
  _is_opcode(token, opcodemap);

  return opcodemap;
}

DirectiveTypeMap *parse_dirtype(const char *token) {
  DirectiveTypeMap *dirtypemap =
      (DirectiveTypeMap *)malloc(sizeof(DirectiveTypeMap));
  _is_directive(token, dirtypemap);

  return dirtypemap;
}

static int _validate_label(const char *token) {
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

static int _is_dec(const char *token) {
  if (token[0] == '#') {
    int i = 2;
    char c = token[i];

    // Special case for negative numbers
    if (!((token[1] >= '0' && token[1] <= '9') ||
          ((token[1] == '-') && token[2] != '\0'))) {
      return 0;
    }

    while (c != '\0') {
      if (!(c >= '0' && c <= '9')) {
        return 0;
      }
      c = token[++i];
    }

    return 1;
  }

  return 0;
}

static int _is_hex(const char *token) {
  if (token[0] == 'X' || (token[0] == '0' && token[1] == 'X')) {
    int i = token[0] == 'X' ? 1 : 2;
    char c = token[i];

    while (c != '\0') {
      if (!((c >= '0' && c <= '9') || (c >= 'A' && c <= 'F'))) {
        return 0;
      }
      c = token[++i];
    }

    return 1;
  }

  return 0;
}

static int _is_register(const char *token, Register *reg) {
  for (int i = 0; i < (int)(sizeof(reg_map) / sizeof(reg_map[0])); ++i) {
    if (strcmp(token, reg_map[i].token) == 0) {
      if (reg) *reg = reg_map[i].reg;
      return 1;
    }
  }

  return 0;
}

static int _is_opcode(const char *token, OperationCodeMap *opcodemap) {
  int i;

  for (i = 0; i < (int)(sizeof(opcode_map) / sizeof(opcode_map[0])); ++i) {
    if (strcmp(token, opcode_map[i].token) == 0) {
      if (opcodemap) *opcodemap = opcode_map[i];
      return 1;
    }
  }

  if (opcodemap) *opcodemap = opcode_map[i - 1];
  return 0;
}

static int _is_directive(const char *token, DirectiveTypeMap *dirtypemap) {
  int i;

  for (i = 0; i < (int)(sizeof(directive_map) / sizeof(directive_map[0]));
       ++i) {
    if (strcmp(token, directive_map[i].token) == 0) {
      if (dirtypemap) *dirtypemap = directive_map[i];
      return 1;
    }
  }

  if (dirtypemap) *dirtypemap = directive_map[i - 1];
  return 0;
}

static Operand *_parse_operand_imm(const char *token, int is_dec) {
  Operand *operand = (Operand *)malloc(sizeof(Operand));
  long int imm;

  if (is_dec) {
    imm = strtol(++token, NULL, 10);
  } else {
    token += token[0] == 'X' ? 1 : 2;
    imm = strtol(token, NULL, 16);
  }

  operand->type = OPERAND_INVALID;

  for (int i = 0; imm <= OFF_BOUNDS[i] - 1 && imm >= OFF_BOUNDS[i] * (-1) &&
                  i <= N_OFF_BOUNDS;
       ++i) {
    operand->type |= OFF_MAP[i];
  }

  operand->operand.imm = (int16_t)imm;

  return operand;
}

static Operand *_parse_operand_reg(Register reg) {
  Operand *operand = (Operand *)malloc(sizeof(Operand));

  operand->type = OPT_REG;
  operand->operand.reg = reg;

  return operand;
}

static Operand *_parse_operand_label(const char *token) {
  Operand *operand = (Operand *)malloc(sizeof(Operand));

  operand->type = OPT_LAB;

  if (!_validate_label(token)) {
    operand->type = OPERAND_INVALID;
    return operand;
  }

  strncpy(operand->operand.label, token, STRLEN - 1);
  operand->operand.label[STRLEN - 1] = '\0';

  return operand;
}

static Operand *_parse_operand_stringz(const char *token) {
  Operand *operand = (Operand *)malloc(sizeof(Operand));

  operand->type = OPT_STRINGZ;
  strncpy(operand->operand.stringz, ++token, STRLEN);
  operand->operand.stringz[STRLEN - 1] = '\0';

  return operand;
}
