#ifndef ASSEMBLER
#define ASSEMBLER

#include "assembler/line.h"

typedef struct {
    Line* head;
    Line* tail;
} LineList;

typedef struct LabelMap LabelMap;
struct LabelMap {
    char label[STRLEN];
    uint16_t addr;
    LabelMap* next;
};

typedef struct {
    LabelMap* head;
    LabelMap* tail;
} LabelList;

void assemble(const char* fpath);

#endif
