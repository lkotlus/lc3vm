#ifndef MAIN
#define MAIN

#include "constants.h"

typedef struct {
    char assemble;
    char disassemble;
    char run;
    char help;
    char inpath[STRLEN];
    char outpath[STRLEN];
} CliFlags;

CliFlags* cliflags_create();
CliFlags* parse_cliflags(int argv, char* args[]);
void validate_path(const char* path);
void print_help();

#endif
