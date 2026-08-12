#ifndef ASSEMBLER
#define ASSEMBLER

#include "assembler/line.h"

typedef struct {
    Line* orig;
    Line* current;
} LineList;

typedef struct LabelMap LabelMap;
struct LabelMap {
    char label[STRLEN];
    uint16_t addr;
    LabelMap* next;
};

typedef struct {
    LabelMap* head;
    LabelMap* current;
} LabelList;

void assemble(const char* fpath);

#endif
