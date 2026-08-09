#if !defined (cliflags)

#define STRLEN 255

typedef struct cliflags cliflags;

struct cliflags {
    char assemble;
    char disassemble;
    char run;
    char help;
    char inpath[STRLEN];
    char outpath[STRLEN];
};

cliflags* cliflags_create();
cliflags* parse_cliflags(int argv, char* args[]);
void validate_path(const char* path);
void print_help();

#endif
