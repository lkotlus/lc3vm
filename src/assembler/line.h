#ifndef LINE
#define LINE

#include <stdio.h>

#include "assembler/token.h"

// Operations
typedef enum {
  OP_ERR_NONE,
  OP_ERR_TOO_MANY_OPERANDS,
  OP_ERR_TOO_FEW_OPERANDS,
  OP_ERR_WRONG_TYPE_OPERANDS
} OperationErr;
extern const char *operation_errs[];

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
extern const char *directive_errs[];

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
  LINE_ERR_DUPLICATE_LABEL,
  LINE_ERR_NO_ORIG,
  LINE_ERR_OP,
  LINE_ERR_DIR,
} LineErr;
extern const char *line_errs[];

typedef enum { LINE_OPERATION, LINE_DIRECTIVE } LineType;
extern const char *line_types[];

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

typedef struct {
  Line *head;
  Line *tail;
} LineList;

Line *parse_line(FILE *f, int addr, LabelList *labell);
int write_instruction(Line *line, LabelList *labell, uint16_t *inst, int i);
void print_line(Line *line);

#endif
