#if !defined (CliFlags)

#define STRLEN 255

typedef struct CliFlags CliFlags;

struct CliFlags {
    char assemble;
    char disassemble;
    char run;
    char help;
    char inpath[STRLEN];
    char outpath[STRLEN];
};

CliFlags* cliflags_create();
CliFlags* parse_cliflags(int argv, char* args[]);
void validate_path(const char* path);
void print_help();

#endif
