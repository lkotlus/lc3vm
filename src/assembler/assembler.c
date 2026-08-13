#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "assembler/line.h"
#include "assembler/token.h"
#include "assembler/assembler.h"

static int _first_pass(FILE* file, LabelList* labell, LineList* linel);
static LabelMap* _label_map_factory(char* label, uint16_t addr);
static void _labell_push(LabelList* labell, LabelMap* lmap);
static void _linel_push(LineList* linel, Line* line);

void assemble(const char* fpath) {
    FILE* file = fopen(fpath, "r");
    LabelList labell = {NULL, NULL};
    LineList linel = {NULL, NULL};
    int errors;

    errors = _first_pass(file, &labell, &linel);

    if (errors) {
        // Do some things.
        printf("HAD ERRORS :(\n");
    }

    fclose(file);
}

static int _first_pass(FILE* file, LabelList* labell, LineList* linel) {
    for (Line* line = parse_line(file, -1); line; line = parse_line(file, (int)(line->addr+1))) {
        if (line->label[0]) {
            _labell_push(labell, _label_map_factory(line->label, line->addr));
        }

        _linel_push(linel, line);

        if (!(line->err == LINE_ERR_NONE)) {
            return 1;
        }

        printf("\n");
    }

    return 0;
}

static LabelMap* _label_map_factory(char* label, uint16_t addr) {
    LabelMap* lmap = (LabelMap*)malloc(sizeof(LabelMap));

    lmap->addr = addr;
    strncpy(lmap->label, label, STRLEN-1);
    lmap->label[STRLEN-1] = '\0';
    lmap->next = NULL;

    return lmap;
}

static void _labell_push(LabelList* labell, LabelMap* lmap) {
    if (!labell->head) {
        labell->head = lmap;
        labell->current = labell->head;
    }
    else {
        labell->current->next = lmap;
        labell->current = labell->current->next;
    }
}
static void _linel_push(LineList* linel, Line* line) {
    if (!linel->orig) {
        linel->orig = line;
        linel->current = line;
    }
    else {
        linel->current->next = line;
        linel->current = linel->current->next;
    }
}
