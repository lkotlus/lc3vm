#ifndef LINE
#define LINE

#include <stdio.h>

#include "assembler/token.h"

#define IMM_STRMAP                                                    \
  ((const char *[]){"OPERAND_INVALID", "OPT_IMM16", "OPT_PCOFFSET11", \
                    "OPT_PCOFFSET9", "OPT_TRAPVECT8", "OPT_OFFSET6",  \
                    "OPT_IMM5"})
#define line_errs                 \
  ((const char *[]){              \
      "LINE_ERR_NONE",            \
      "LINE_ERR_INVALID_TOKEN",   \
      "LINE_ERR_DUPLICATE_LABEL", \
      "LINE_ERR_NO_ORIG",         \
      "LINE_ERR_OP",              \
      "LINE_ERR_DIR",             \
  })
#define operation_errs                                         \
  ((const char *[]){"OP_ERR_NONE", "OP_ERR_TOO_MANY_OPERANDS", \
                    "OP_ERR_TOO_FEW_OPERANDS", "OP_ERR_WRONG_TYPE_OPERANDS"})
#define directive_errs                                           \
  ((const char *[]){"DIR_ERR_NONE", "DIR_ERR_TOO_MANY_OPERANDS", \
                    "DIR_ERR_TOO_FEW_OPERANDS",                  \
                    "DIR_ERR_WRONG_TYPE_OPERANDS"})

// Operations
typedef enum {
  OP_ERR_NONE,
  OP_ERR_TOO_MANY_OPERANDS,
  OP_ERR_TOO_FEW_OPERANDS,
  OP_ERR_WRONG_TYPE_OPERANDS,
  OP_ERR_NOMATCH_LABEL
} OperationErr;

typedef struct {
  OperationErr err;
  int n_ops;
  Operand *operands[3];
  OperationCode opcode;
} Operation;

// Directives
typedef enum {
  DIR_ERR_NONE,
  DIR_ERR_TOO_MANY_OPERANDS,
  DIR_ERR_TOO_FEW_OPERANDS,
  DIR_ERR_WRONG_TYPE_OPERANDS
} DirectiveErr;

typedef struct {
  DirectiveErr err;
  DirectiveType type;
  int has_operand;
  Operand *operand;
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

typedef enum { LINE_OPERATION, LINE_DIRECTIVE } LineType;

typedef struct Line Line;
struct Line {
  LineErr err;
  uint16_t addr;
  char label[STRLEN];
  LineType type;
  union {
    Directive *directive;
    Operation *operation;
  } line;
  Line *next;
};

typedef struct {
  Line *head;
  Line *tail;
} LineList;

Line *parse_line(FILE *f, int addr, LabelList *labell);
int write_instructions(Line *line, LabelList *labell, uint16_t *inst, int i);
void print_line(Line *line);

#endif
