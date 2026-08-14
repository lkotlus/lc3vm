#ifndef TOKEN
#define TOKEN

#include <limits.h>
#include <stdint.h>

#include "constants.h"

#define GMAX_INT16 0x10000
#define MAX_INT16 0xFFFF
#define MAX_INT11 0x7FF
#define MAX_INT9 0x1FF
#define MAX_INT8 0xFF
#define MAX_INT6 0x3F
#define MAX_INT5 0x1F
#define N_MAX_INTS 6
#define MAX_INTS                                                            \
  ((const long int[]){GMAX_INT16, MAX_INT16, MAX_INT11, MAX_INT9, MAX_INT8, \
                      MAX_INT6, MAX_INT5})
#define IMM_MAP                                                             \
  ((const int[]){OPERAND_INVALID, OPT_IMM16, OPT_PCOFFSET11, OPT_PCOFFSET9, \
                 OPT_TRAPVECT8, OPT_OFFSET6, OPT_IMM5})
#define IMM_STRMAP                                                    \
  ((const char *[]){"OPERAND_INVALID", "OPT_IMM16", "OPT_PCOFFSET11", \
                    "OPT_PCOFFSET9", "OPT_TRAPVECT8", "OPT_OFFSET6",  \
                    "OPT_IMM5"})
#define OPT_IMM_OFFSET                                                       \
  (OPT_IMM5 | OPT_OFFSET6 | OPT_TRAPVECT8 | OPT_PCOFFSET9 | OPT_PCOFFSET11 | \
   OPT_IMM16)

typedef struct LabelMap LabelMap;
struct LabelMap {
  char label[STRLEN];
  uint16_t addr;
  LabelMap *next;
};

typedef struct {
  LabelMap *head;
  LabelMap *tail;
} LabelList;

typedef enum {
  LABEL_VALID,
  LABEL_INVALID_DUPLICATE,
  LABEL_INVALID_CHARS
} LabelResult;

LabelResult parse_label(const char *token, LabelList *labell);

// Operands
typedef enum {
  OPT_LAB = 1 << 0,
  OPT_REG = 1 << 1,
  OPT_IMM5 = 1 << 2,
  OPT_PCOFFSET9 = 1 << 3,
  OPT_PCOFFSET11 = 1 << 4,
  OPT_OFFSET6 = 1 << 5,
  OPT_TRAPVECT8 = 1 << 6,
  OPT_STRINGZ = 1 << 7,
  OPT_IMM16 = 1 << 8,
  OPERAND_INVALID = 0
} OperandType;

typedef enum { R0, R1, R2, R3, R4, R5, R6, R7 } Register;

typedef struct {
  OperandType type;
  union {
    int16_t imm;
    Register reg;
    char label[STRLEN];
    char stringz[STRLEN];
  } operand;
} Operand;

typedef struct {
  const char *token;
  Register reg;
} RegisterMap;

static const RegisterMap reg_map[] = {{"R0", R0}, {"R1", R1}, {"R2", R2},
                                      {"R3", R3}, {"R4", R4}, {"R5", R5},
                                      {"R6", R6}, {"R7", R7}};

Operand *parse_operand(const char *token);

typedef enum {
  ADD,
  AND,
  NOT,
  LD,
  LDI,
  LDR,
  LEA,
  ST,
  STR,
  STI,
  BR,
  BRN,
  BRZ,
  BRP,
  BRNZ,
  BRZP,
  BRNP,
  JSR,
  JSRR,
  JMP,
  RTI,
  TRAP,
  GETC,
  OUT,
  PUTS,
  IN,
  PUTSP,
  HALT,
  OPCODE_INVALID
} OperationCode;

typedef struct {
  int n_ops;
  OperandType op_types[3];
} OperationSpec;

typedef struct {
  const char *token;
  OperationCode opcode;
  OperationSpec opspec;
} OperationCodeMap;

static const OperationCodeMap opcode_map[] = {
    {"ADD", ADD, {3, {OPT_REG, OPT_REG, OPT_REG | OPT_IMM5}}},
    {"AND", AND, {3, {OPT_REG, OPT_REG, OPT_REG | OPT_IMM5}}},
    {"NOT", NOT, {2, {OPT_REG, OPT_REG, 0}}},
    {"LD", LD, {2, {OPT_REG, OPT_LAB | OPT_PCOFFSET9, 0}}},
    {"LDI", LDI, {2, {OPT_REG, OPT_LAB | OPT_PCOFFSET9, 0}}},
    {"LDR", LDR, {3, {OPT_REG, OPT_REG, OPT_OFFSET6}}},
    {"LEA", LEA, {2, {OPT_REG, OPT_LAB | OPT_PCOFFSET9, 0}}},
    {"ST", ST, {2, {OPT_REG, OPT_LAB | OPT_PCOFFSET9, 0}}},
    {"STR", STR, {3, {OPT_REG, OPT_REG, OPT_OFFSET6}}},
    {"STI", STI, {2, {OPT_REG, OPT_LAB | OPT_PCOFFSET9, 0}}},
    {"BR", BR, {1, {OPT_LAB | OPT_PCOFFSET9, 0, 0}}},
    {"BRN", BRN, {1, {OPT_LAB | OPT_PCOFFSET9, 0, 0}}},
    {"BRZ", BRZ, {1, {OPT_LAB | OPT_PCOFFSET9, 0, 0}}},
    {"BRP", BRP, {1, {OPT_LAB | OPT_PCOFFSET9, 0, 0}}},
    {"BRNZ", BRNZ, {1, {OPT_LAB | OPT_PCOFFSET9, 0, 0}}},
    {"BRZP", BRZP, {1, {OPT_LAB | OPT_PCOFFSET9, 0, 0}}},
    {"BRNP", BRNP, {1, {OPT_LAB | OPT_PCOFFSET9, 0, 0}}},
    {"JSR", JSR, {1, {OPT_LAB | OPT_PCOFFSET11, 0, 0}}},
    {"JSRR", JSRR, {1, {OPT_REG, 0, 0}}},
    {"JMP", JMP, {1, {OPT_REG, 0, 0}}},
    {"RTI", RTI, {0, {0, 0, 0}}},
    {"TRAP", TRAP, {1, {OPT_TRAPVECT8, 0, 0}}},
    {"GETC", GETC, {0, {0, 0, 0}}},
    {"OUT", OUT, {0, {0, 0, 0}}},
    {"PUTS", PUTS, {0, {0, 0, 0}}},
    {"IN", IN, {0, {0, 0, 0}}},
    {"PUTSP", PUTSP, {0, {0, 0, 0}}},
    {"HALT", HALT, {0, {0, 0, 0}}},
    {"", OPCODE_INVALID, {0, {0, 0, 0}}}};

OperationCodeMap *parse_opcode(const char *token);

// Directives
typedef enum {
  ORIG,
  END,
  FILL,
  BLKW,
  STRINGZ,
  DIRECTIVE_INVALID
} DirectiveType;

typedef struct {
  const char *token;
  DirectiveType dirtype;
  OperandType operand;
} DirectiveTypeMap;

static const DirectiveTypeMap directive_map[] = {
    {".ORIG", ORIG, OPT_IMM16},
    {".END", END, OPERAND_INVALID},
    {".FILL", FILL, OPT_LAB | OPT_IMM16},
    {".BLKW", BLKW, OPT_IMM16},
    {".STRINGZ", STRINGZ, OPT_STRINGZ},
    {"", DIRECTIVE_INVALID, OPERAND_INVALID}};

DirectiveTypeMap *parse_dirtype(const char *token);

#endif
