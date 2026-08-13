#include "assembler/line.h"
#include "assembler/token.h"
#include "constants.h"
#include <ctype.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const char *line_errs[] = {
    "LINE_ERR_NONE", "LINE_ERR_INVALID_TOKEN", "LINE_ERR_NO_ORIG",
    "LINE_ERR_OP",   "LINE_ERR_DIR",
};
const char *operation_errs[] = {
    "OP_ERR_NONE", "OP_ERR_TOO_MANY_OPERANDS", "OP_ERR_TOO_FEW_OPERANDS",
    "OP_ERR_WRONG_TYPE_OPERANDS"};
const char *directive_errs[] = {
    "DIR_ERR_NONE", "DIR_ERR_TOO_MANY_OPERANDS", "DIR_ERR_TOO_FEW_OPERANDS",
    "DIR_ERR_WRONG_TYPE_OPERANDS"};
const char *line_types[] = {"LINE_OPERATION", "LINE_DIRECTIVE"};

// Gets the next line of a file that contains a token
static Line *_line_factory();
static int _get_fline(FILE *f, char *line);
static void _skip_comment(FILE *f, int *ch);
static int _get_token(char **line, char *token);
static Operation *_get_operation(char **fl, OperationCodeMap *opcodemap);
static Directive *_get_directive(char **fl, DirectiveTypeMap *dirtypemap);
static void _print_operation(Line *line);
static void _print_directive(Line *line);

Line *parse_line(FILE *f, int addr) {
  Line *line = _line_factory();
  char fline[STRLEN];

  if (!_get_fline(f, fline)) {
    return NULL;
  }

  char token[STRLEN];
  char *fl = fline;

  _get_token(&fl, token);

  if (parse_label(token) == LABEL_VALID) {
    strncpy(line->label, token, STRLEN - 1);
    line->label[STRLEN - 1] = '\0';

    _get_token(&fl, token);
  }

  OperationCodeMap *opcodemap = parse_opcode(token);
  DirectiveTypeMap *dirtypemap = parse_dirtype(token);
  if (opcodemap->opcode != OPCODE_INVALID) {
    Operation *op = _get_operation(&fl, opcodemap);
    line->type = LINE_OPERATION;
    line->line.operation = *op;

    if (op->err != OP_ERR_NONE)
      line->err = LINE_ERR_OP;
  } else if (dirtypemap->dirtype != DIRECTIVE_INVALID) {
    Directive *dir = _get_directive(&fl, dirtypemap);
    line->type = LINE_DIRECTIVE;
    line->line.directive = *dir;

    if (dir->err != DIR_ERR_NONE)
      line->err = LINE_ERR_DIR;
  } else {
    line->err = LINE_ERR_INVALID_TOKEN;
    return line;
  }

  if (addr < 0) {
    if (line->line.directive.type != ORIG)
      line->err = LINE_ERR_NO_ORIG;
    else
      line->addr = line->line.directive.operand.operand.ival;
  } else
    line->addr = (uint16_t)addr;

  return line;
}

void print_line(Line *line) {
  printf("%s\t", line->label);

  if (line->type == LINE_OPERATION) {
    _print_operation(line);
  } else {
    _print_directive(line);
  }
}

static Line *_line_factory() {
  Line *line = (Line *)malloc(sizeof(Line));

  line->err = LINE_ERR_NONE;
  line->label[0] = '\0';
  line->next = NULL;

  return line;
}

static int _get_fline(FILE *f, char *fline) {
  int ch;
  int i = 0;

  while (((ch = toupper(fgetc(f))) != EOF) &&
         (isspace((unsigned char)ch) || ch == ';')) {
    _skip_comment(f, &ch);
  }

  if (ch == EOF) {
    return 0;
  }

  while (ch != ';' && ch != '\n') {
    if (i < STRLEN - 1) {
      fline[i++] = (unsigned char)ch;
    }

    ch = toupper(fgetc(f));
  }
  _skip_comment(f, &ch);

  fline[i] = '\0';

  return 1;
}

static void _skip_comment(FILE *f, int *ch) {
  if (*ch == ';') {
    while (((*ch = toupper(fgetc(f))) != EOF) && *ch != '\n')
      ;
  }
}

static int _get_token(char **fline, char *token) {
  int i = 0;

  while (**fline != '\0') {
    while (!isspace(**fline) && **fline != ',' && **fline != '\0') {
      token[i++] = *++(*fline);
    }
    if (i > 0) {
      token[i] = '\0';
      return 1;
    }

    ++(*fline);
  }

  token[i] = '\0';
  return 0;
}

static Operation *_get_operation(char **fl, OperationCodeMap *opcodemap) {
  char token[STRLEN];
  Operation *op = (Operation *)malloc(sizeof(Operation));
  op->err = OP_ERR_NONE;
  op->opcode = opcodemap->opcode;
  op->n_ops = opcodemap->opspec.n_ops;

  for (int i = 0; i < opcodemap->opspec.n_ops; ++i) {
    Operand *operand;

    if (!_get_token(fl, token)) {
      op->err = OP_ERR_TOO_FEW_OPERANDS;
      return op;
    }

    operand = parse_operand(token);

    // Some serious voodoo going on here.
    if (!(operand->type & opcodemap->opspec.op_types[i])) {
      op->err = OP_ERR_WRONG_TYPE_OPERANDS;
      return op;
    }

    op->operands[i] = *operand;
  }

  if (_get_token(fl, token))
    op->err = OP_ERR_TOO_MANY_OPERANDS;

  return op;
}

static Directive *_get_directive(char **fl, DirectiveTypeMap *dirtypemap) {
  char token[STRLEN];
  Directive *dir = (Directive *)malloc(sizeof(Directive));
  dir->err = DIR_ERR_NONE;
  dir->type = dirtypemap->dirtype;
  dir->has_operand = dirtypemap->operand;

  if (dirtypemap->operand) {
    Operand *operand;

    if (!_get_token(fl, token)) {
      dir->err = DIR_ERR_TOO_FEW_OPERANDS;
      return dir;
    }

    operand = parse_operand(token);

    // Some serious voodoo going on here.
    if (!(operand->type & dirtypemap->operand)) {
      dir->err = DIR_ERR_WRONG_TYPE_OPERANDS;
      return dir;
    }

    dir->operand = *operand;
  }

  return dir;
}

static void _print_operation(Line *line) {
  for (int i = 0; i < (int)(sizeof(opcode_map) / sizeof(opcode_map[0])); ++i) {
    if (line->line.operation.opcode == opcode_map[i].opcode) {
      printf("%s ", opcode_map[i].token);
      break;
    }
  }

  for (int i = 0; i < line->line.operation.n_ops; ++i) {
    switch (line->line.operation.operands[i].type) {
    case OPT_IVA:
      printf("0x%x", line->line.operation.operands[i].operand.ival);
      break;
    case OPT_REG:
      for (int j = 0; j < (int)(sizeof(reg_map) / sizeof(reg_map[0])); ++j) {
        if (line->line.operation.operands[i].operand.reg == reg_map[j].reg) {
          printf("%s", reg_map[j].token);
        }
      }
      break;
    case OPT_LAB:
      printf("%s", line->line.operation.operands[i].operand.label);
      break;
    case OPT_STR:
      break;
    case OPERAND_INVALID:
      break;
    }

    if (i < line->line.operation.n_ops - 1) {
      printf(", ");
    }
  }
}

static void _print_directive(Line *line) {
  for (int i = 0; i < (int)(sizeof(directive_map) / sizeof(directive_map[0]));
       ++i) {
    if (line->line.directive.type == directive_map[i].dirtype) {
      printf("%s ", directive_map[i].token);
      break;
    }
  }

  if (line->line.directive.has_operand) {
    switch (line->line.directive.operand.type) {
    case OPT_IVA:
      printf("0x%x", line->line.directive.operand.operand.ival);
      break;
    case OPT_REG:
      break;
    case OPT_LAB:
      printf("%s", line->line.directive.operand.operand.label);
      break;
    case OPT_STR:
      printf("%s", line->line.directive.operand.operand.stringz);
      break;
    case OPERAND_INVALID:
      break;
    }
  }
}
