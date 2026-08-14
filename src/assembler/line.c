#include "assembler/line.h"

#include <ctype.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "assembler/token.h"
#include "constants.h"

const char *line_errs[] = {
    "LINE_ERR_NONE",
    "LINE_ERR_INVALID_TOKEN",
    "LINE_ERR_DUPLICATE_LABEL",
    "LINE_ERR_NO_ORIG",
    "LINE_ERR_OP",
    "LINE_ERR_DIR",
};
const char *operation_errs[] = {"OP_ERR_NONE", "OP_ERR_TOO_MANY_OPERANDS",
                                "OP_ERR_TOO_FEW_OPERANDS",
                                "OP_ERR_WRONG_TYPE_OPERANDS"};
const char *directive_errs[] = {"DIR_ERR_NONE", "DIR_ERR_TOO_MANY_OPERANDS",
                                "DIR_ERR_TOO_FEW_OPERANDS",
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
static int _assemble_operation(Line *line, LabelList *labell, uint16_t inst[],
                               int i);
static int _assemble_directive(Line *line, LabelList *labell, uint16_t inst[],
                               int i);
static void _convert_label(Line *label_operand, LabelList *labell, int i);
static void _convert_label_operation(Line *line, LabelList *labell, int i);
static void _convert_label_directive(Line *line, LabelList *labell);

Line *parse_line(FILE *f, int addr, LabelList *labell) {
  Line *line = _line_factory();
  char fline[STRLEN];

  if (!_get_fline(f, fline)) {
    return NULL;
  }

  char token[STRLEN];
  char *fl = fline;

  _get_token(&fl, token);

  if (parse_label(token, labell) == LABEL_VALID) {
    strncpy(line->label, token, STRLEN - 1);
    line->label[STRLEN - 1] = '\0';

    _get_token(&fl, token);
  }

  OperationCodeMap *opcodemap = parse_opcode(token);
  DirectiveTypeMap *dirtypemap = parse_dirtype(token);
  if (opcodemap->opcode != OPCODE_INVALID) {
    Operation *op = _get_operation(&fl, opcodemap);
    line->type = LINE_OPERATION;
    line->line.operation = op;

    if (op->err != OP_ERR_NONE) line->err = LINE_ERR_OP;
  } else if (dirtypemap->dirtype != DIRECTIVE_INVALID) {
    Directive *dir = _get_directive(&fl, dirtypemap);
    line->type = LINE_DIRECTIVE;
    line->line.directive = dir;

    if (dir->err != DIR_ERR_NONE) line->err = LINE_ERR_DIR;
  } else {
    line->err = LINE_ERR_INVALID_TOKEN;
    return line;
  }

  if (line->err != LINE_ERR_NONE) {
    return line;
  }

  if (addr < 0) {
    if (line->line.directive->type != ORIG) {
      line->err = LINE_ERR_NO_ORIG;
    } else {
      line->addr = line->line.directive->operand->operand.imm;
    }
  } else {
    line->addr = (uint16_t)addr;
  }

  return line;
}

int write_instructions(Line *line, LabelList *labell, uint16_t *inst, int i) {
  if (line->type == LINE_OPERATION) {
    return _assemble_operation(line, labell, inst, i);
  } else {
    return _assemble_directive(line, labell, inst, i);
  }
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

  while (((ch = fgetc(f)) != EOF) &&
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

    ch = fgetc(f);
  }
  _skip_comment(f, &ch);

  fline[i] = '\0';

  return 1;
}

static void _skip_comment(FILE *f, int *ch) {
  if (*ch == ';') {
    while (((*ch = fgetc(f)) != EOF) && *ch != '\n');
  }
}

static int _get_token(char **fline, char *token) {
  int i = 0;

  while (**fline != '\0') {
    while (isspace(**fline) || **fline == ',') {
      ++(*fline);
    }
    if (**fline == '\0') {
      break;
    }

    // Deal with .STRINGZ
    if (**fline == '"') {
      token[i++] = *((*fline)++);

      while (**fline != '\0' && **fline != '"') {
        if (**fline == '\\' && *((*fline) + 1) != '\0') {
          ++(*fline);
          char c = *((*fline)++);
          switch (c) {
            case 'n':
              token[i++] = '\n';
              break;
            case 't':
              token[i++] = '\t';
              break;
            case 'r':
              token[i++] = '\r';
              break;
            case '\\':
              token[i++] = '\\';
              break;
            case '"':
              token[i++] = '"';
              break;
            default:
              token[i++] = c;
              break;
          }
        } else {
          token[i++] = *((*fline)++);  // Someone ought to lock me up for this.
        }
      }

      token[i] = '\0';
      return 1;
    }

    while (!isspace(**fline) && **fline != ',' && **fline != '\0' &&
           **fline != '"') {
      token[i++] =
          toupper(*((*fline)++));  // Someone ought to lock me up for this.
    }
    token[i] = '\0';
    return 1;
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

    op->operands[i] = operand;
  }

  if (_get_token(fl, token)) op->err = OP_ERR_TOO_MANY_OPERANDS;

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

    dir->operand = operand;
  }

  return dir;
}

static void _print_operation(Line *line) {
  printf("%s ", opcode_map[line->line.operation->opcode].token);

  for (int i = 0; i < line->line.operation->n_ops; ++i) {
    if (line->line.operation->operands[i]->type & OPT_IMM_OFFSET) {
      printf("0x%04hx", (int16_t)line->line.operation->operands[i]->operand.imm);
    } else if (line->line.operation->operands[i]->type & OPT_REG) {
      printf("%s", reg_map[line->line.operation->operands[i]->operand.reg].token);
    } else if (line->line.operation->operands[i]->type & OPT_LAB) {
      printf("%s", line->line.operation->operands[i]->operand.label);
    }

    if (i < line->line.operation->n_ops - 1) {
      printf(", ");
    }
  }

  printf("\n");
}

static void _print_directive(Line *line) {
  printf("%s ", directive_map[line->line.directive->type].token);

  if (line->line.directive->has_operand) {
    if (line->line.directive->operand->type & OPT_IMM_OFFSET) {
      printf("0x%04hx", (int16_t)line->line.directive->operand->operand.imm);
    } else if (line->line.directive->operand->type & OPT_LAB) {
      printf("%s", line->line.directive->operand->operand.label);
    } else if (line->line.directive->operand->type & OPT_STRINGZ) {
      printf("\"%s\"", line->line.directive->operand->operand.stringz);
    }
  }

  printf("\n");
}

static int _assemble_operation(Line *line, LabelList *labell, uint16_t inst[],
                               int i) {
  for (int i = 0; i < line->line.operation->n_ops; ++i) {
    if (line->line.operation->operands[i]->type == OPT_LAB) {
      _convert_label(line, labell, i);
    }
  }

  inst[i] = 0x0000;
  return 1;
}

static int _assemble_directive(Line *line, LabelList *labell, uint16_t inst[],
                               int i) {
  if (line->line.directive->has_operand &&
      line->line.directive->operand->type == OPT_LAB) {
    _convert_label(line, labell, i);
  }

  switch (line->line.directive->type) {
    case ORIG:
      inst[i] = line->line.directive->operand->operand.imm;
      return 1;
    case FILL:
      inst[i] = line->line.directive->operand->operand.imm;
      return 1;
    case BLKW:
      for (int j = 0; j < line->line.directive->operand->operand.imm; ++j) {
        inst[i + j] = 0x0000;
      }
      return line->line.directive->operand->operand.imm;
    case STRINGZ:
      for (int j = 0;
           j < (int)strlen(line->line.directive->operand->operand.stringz);
           ++j) {
        inst[i + j] =
            (uint16_t)line->line.directive->operand->operand.stringz[j];
      }
      return strlen(line->line.directive->operand->operand.stringz);
    case END:
      return 0;
    case DIRECTIVE_INVALID:
      return 0;
  }
}

static void _convert_label(Line *line, LabelList *labell, int i) {
  if (line->type == LINE_OPERATION)
    _convert_label_operation(line, labell, i);
  else
    _convert_label_directive(line, labell);
}

static void _convert_label_operation(Line *line, LabelList *labell, int i) {
  LabelMap *current = labell->head;

  while (strncmp(line->line.operation->operands[i]->operand.label,
                 current->label, STRLEN)) {
    current = current->next;
  }

  if (!current) {
    line->line.operation->err = OP_ERR_NOMATCH_LABEL;
  } else {
    line->line.operation->operands[i]->operand.imm =
        (int16_t)(current->addr - (line->addr + 1));

    for (int j = 0; line->line.operation->operands[i]->operand.imm <= MAX_INTS[i] && j <= N_MAX_INTS; ++j) {
      line->line.operation->operands[i]->type |= IMM_MAP[j];
    }
  }
}

static void _convert_label_directive(Line *line, LabelList *labell) {
  LabelMap *current = labell->head;
  line->line.directive->operand->type = OPT_IMM16;

  while (strncmp(line->line.directive->operand->operand.label, current->label,
                 STRLEN)) {
    current = current->next;
  }

  if (!current) {
    line->line.operation->err = OP_ERR_NOMATCH_LABEL;
  } else {
    line->line.directive->operand->operand.imm = current->addr;
  }
}
