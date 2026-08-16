#include "assembler/assembler.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "assembler/line.h"
#include "assembler/token.h"

static int _first_pass(FILE *file, LabelList *labell, LineList *linel);
static int _second_pass(LineList *linel, LabelList *labell, uint16_t inst[]);
static void _write_assembly(const char *fpath, uint16_t inst[], int words);
static LabelMap *_label_map_factory(char *label, uint16_t addr);
static void _labell_push(LabelList *labell, LabelMap *lmap);
static void _linel_push(LineList *linel, Line *line);
static void _free_labell(LabelList *labell);
static void _free_linel(LineList *linel);
static void _free_operation(Operation *op);
static void _free_directive(Directive *dir);
static void _print_labell(LabelList *labell);
static void _print_linel(LineList *linel);

void assemble(const char *ifpath, const char *ofpath) {
  FILE *ifile = fopen(ifpath, "r");
  LabelList labell = {NULL, NULL};
  LineList linel = {NULL, NULL};

  int words = _first_pass(ifile, &labell, &linel);

  if (words < 0) {
    printf("ENCOUNTERED ERRORS!\n");
    printf("\tLine error: %s\n", line_errs[linel.tail->err]);

    if (linel.tail->type == LINE_OPERATION) {
      printf("\tOperation error: %s\n",
             operation_errs[linel.tail->line.operation->err]);
    } else {
      printf("\tDirective error: %s\n",
             directive_errs[linel.tail->line.directive->err]);
    }

    printf("\nProcessed up to:\n");
    _print_linel(&linel);

    printf("\nError occurred on the bottom line.\n");

    _free_labell(&labell);
    _free_linel(&linel);
    fclose(ifile);

    return;
  }

  uint16_t *instructions = (uint16_t *)malloc(sizeof(uint16_t) * words);
  _second_pass(&linel, &labell, instructions);

  _print_labell(&labell);
  printf("\n");
  _print_linel(&linel);

  _write_assembly(ofpath, instructions, words);

  _free_labell(&labell);
  _free_linel(&linel);
  free(instructions);

  fclose(ifile);
}

static int _first_pass(FILE *file, LabelList *labell, LineList *linel) {
  int words = 0;

  for (Line *line = parse_line(file, -1, labell); line;
       line = parse_line(file, (int)(line->addr + 1), labell)) {
    if (line->label[0]) {
      _labell_push(labell, _label_map_factory(line->label, line->addr));
    }

    _linel_push(linel, line);

    if (!(line->err == LINE_ERR_NONE)) {
      return -1;
    }

    if (line->type == LINE_OPERATION) {
      ++words;
    } else {
      Directive *dir = line->line.directive;
      switch (dir->type) {
        case ORIG:
          ++words;
          break;
        case FILL:
          ++words;
          break;
        case BLKW:
          words += dir->operand->operand.imm;
          break;
        case STRINGZ:
          words += strlen(dir->operand->operand.stringz);
          break;
        case END:
          return words;
        case DIRECTIVE_INVALID:
          break;
      }
    }
  }

  return words;
}

static int _second_pass(LineList *linel, LabelList *labell, uint16_t inst[]) {
  Line *current = linel->head;
  int i = 0;

  while (current) {
    i += write_instructions(current, labell, inst, i);
    current = current->next;
  }

  return 0;
}

static void _write_assembly(const char *fpath, uint16_t inst[], int words) {
  FILE *file = fopen(fpath, "wb");

  if (!file) {
    printf("Could not write to output file: %s\n", fpath);
    return;
  }

  int bytes = (int)fwrite(inst, 1, words * 2, file);
  if (bytes != words * 2) {
    printf("Only able to write %d of %d bytes to output file.\n", bytes,
           words * 2);
    return;
  }

  printf("Successfully wrote assembled code to output file: %s\n", fpath);
  fclose(file);
}

static LabelMap *_label_map_factory(char *label, uint16_t addr) {
  LabelMap *lmap = (LabelMap *)malloc(sizeof(LabelMap));

  lmap->addr = addr;
  strncpy(lmap->label, label, STRLEN - 1);
  lmap->label[STRLEN - 1] = '\0';
  lmap->next = NULL;

  return lmap;
}

static void _labell_push(LabelList *labell, LabelMap *lmap) {
  if (!labell->head) {
    labell->head = lmap;
    labell->tail = lmap;
  } else {
    labell->tail->next = lmap;
    labell->tail = labell->tail->next;
  }
}

static void _linel_push(LineList *linel, Line *line) {
  if (!linel->head) {
    linel->head = line;
    linel->tail = line;
  } else {
    linel->tail->next = line;
    linel->tail = linel->tail->next;
  }
}

static void _free_labell(LabelList *labell) {
  LabelMap *old = labell->head;
  LabelMap *current = labell->head;

  while (current) {
    old = current;
    current = current->next;

    free(old);
  }
}

static void _free_linel(LineList *linel) {
  Line *old = linel->head;
  Line *current = linel->head;

  while (current) {
    old = current;
    current = current->next;

    if (old->type == LINE_OPERATION) {
      _free_operation(old->line.operation);
    } else {
      _free_directive(old->line.directive);
    }

    free(old);
  }
}

static void _free_operation(Operation *op) {
  for (int i = 0; i < op->n_ops; ++i) {
    free(op->operands[i]);
  }

  free(op);
}

static void _free_directive(Directive *dir) {
  if (dir->has_operand) {
    free(dir->operand);
  }

  free(dir);
}

static void _print_labell(LabelList *labell) {
  LabelMap *current = labell->head;

  while (current) {
    printf("%s\t0x%x\n", current->label, current->addr);
    current = current->next;
  }
}

static void _print_linel(LineList *linel) {
  Line *current = linel->head;

  while (current) {
    print_line(current);
    current = current->next;
  }
}
