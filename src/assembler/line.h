#ifndef LINE
#define LINE

#include "assembler/token.h"
#include <stdio.h>

// Operations
typedef enum {
  OP_ERR_NONE,
  OP_ERR_TOO_MANY_OPERANDS,
  OP_ERR_TOO_FEW_OPERANDS,
  OP_ERR_WRONG_TYPE_OPERANDS
} OperationErr;
static const char *operation_errs[] = {
    "OP_ERR_NONE", "OP_ERR_TOO_MANY_OPERANDS", "OP_ERR_TOO_FEW_OPERANDS",
    "OP_ERR_WRONG_TYPE_OPERANDS"};

typedef struct {
  OperationErr err;
  int n_ops;
  Operand operands[3];
  OperationCode opcode;
} Operation;

// Directives
typedef enum {
  DIR_ERR_NONE,
  DIR_ERR_TOO_MANY_OPERANDS,
  DIR_ERR_TOO_FEW_OPERANDS,
  DIR_ERR_WRONG_TYPE_OPERANDS
} DirectiveErr;
static const char *directive_errs[] = {
    "DIR_ERR_NONE", "DIR_ERR_TOO_MANY_OPERANDS", "DIR_ERR_TOO_FEW_OPERANDS",
    "DIR_ERR_WRONG_TYPE_OPERANDS"};

typedef struct {
  DirectiveErr err;
  DirectiveType type;
  int has_operand;
  Operand operand;
} Directive;

// Lines
typedef enum {
  LINE_ERR_NONE,
  LINE_ERR_INVALID_TOKEN,
  LINE_ERR_NO_ORIG,
  LINE_ERR_OP,
  LINE_ERR_DIR,
} LineErr;
static const char *line_errs[] = {
    "LINE_ERR_NONE", "LINE_ERR_INVALID_TOKEN", "LINE_ERR_NO_ORIG",
    "LINE_ERR_OP",   "LINE_ERR_DIR",
};

typedef enum { LINE_OPERATION, LINE_DIRECTIVE } LineType;
static const char *line_types[] = {"LINE_OPERATION", "LINE_DIRECTIVE"};

typedef struct Line Line;
struct Line {
  LineErr err;
  uint16_t addr;
  char label[STRLEN];
  LineType type;
  union {
    Directive directive;
    Operation operation;
  } line;
  Line *next;
};

Line *parse_line(FILE *f, int addr);
void print_line(Line *line);

#endif
