#include "assembler/assembler.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "assembler/line.h"
#include "assembler/token.h"

static int _first_pass(FILE *file, LabelList *labell, LineList *linel);
static LabelMap *_label_map_factory(char *label, uint16_t addr);
static void _labell_push(LabelList *labell, LabelMap *lmap);
static void _linel_push(LineList *linel, Line *line);
static void _free_labell(LabelList *labell);
static void _free_linel(LineList *linel);
static void _print_labell(LabelList *labell);
static void _print_linel(LineList *linel);

void assemble(const char *fpath) {
  FILE *file = fopen(fpath, "r");
  LabelList labell = {NULL, NULL};
  LineList linel = {NULL, NULL};

  int n_lines = _first_pass(file, &labell, &linel);

  if (n_lines < 0) {
    // Do some things.
    printf("HAD ERRORS :(\n");
    return;
  }

  _print_labell(&labell);
  _print_linel(&linel);

  // uint16_t instructions[n_lines];

  _free_labell(&labell);
  _free_linel(&linel);

  fclose(file);
}

static int _first_pass(FILE *file, LabelList *labell, LineList *linel) {
  int n_lines = 0;

  for (Line *line = parse_line(file, -1); line;
       line = parse_line(file, (int)(line->addr + 1))) {
    if (line->label[0]) {
      _labell_push(labell, _label_map_factory(line->label, line->addr));
    }

    _linel_push(linel, line);

    if (!(line->err == LINE_ERR_NONE)) {
      printf("%s\n", line_types[line->type]);
      printf("%s\n", line_errs[line->err]);
      printf("%s\n", operation_errs[line->line.operation.err]);
      printf("%s\n", directive_errs[line->line.directive.err]);
      return -1;
    }

    ++n_lines;
  }

  return n_lines;
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

    free(old);
  }
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
