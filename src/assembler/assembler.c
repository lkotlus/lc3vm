#include <stdlib.h>
#include <stdio.h>
#include "assembler/line.h"

void assemble(const char* fpath) {
    FILE* file = fopen(fpath, "r");
    
    for (Line* line = parse_line(file); line; line = parse_line(file)) {
        free(line);
    };

    fclose(file);
}
