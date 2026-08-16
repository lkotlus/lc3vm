#include "main.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "assembler/assembler.h"

int main(int argv, char* args[]) {
  CliFlags* flags = parse_cliflags(argv, args);

  if (flags->help) {
    print_help();
    exit(0);
  } else if (flags->assemble) {
    assemble(flags->inpath, flags->outpath);
  } else if (flags->disassemble) {
    printf("Disassembling %s...\n", flags->inpath);
    printf("Disassembled code located in %s\n", flags->outpath);
  } else {
    printf("Running %s...\n", flags->inpath);
  }

  free(flags);

  return 0;
}

CliFlags* cliflags_create() {
  CliFlags* flags = (CliFlags*)malloc(sizeof(CliFlags));

  flags->assemble = 0;
  flags->disassemble = 0;
  flags->run = 0;
  flags->help = 0;

  return flags;
}

CliFlags* parse_cliflags(int argv, char* args[]) {
  CliFlags* flags = cliflags_create();

  if (argv < 2) {
    printf("You must provide at least one command line argument.\n\n");
    print_help();
    exit(-1);
  }

  if (args[1][0] != '-') {
    printf("You must provide at least one command line argument.\n\n");
    print_help();
    exit(-1);
  }

  switch (args[1][1]) {
    case 'h':
      flags->help = 1;
      break;
    case 'a':
      flags->assemble = 1;
      if (argv != 4) {
        printf("Assembly requires 3 command line arguments.\n\n");
        print_help();
        exit(-1);
      }
      validate_path(args[2], 0);
      strncpy(flags->inpath, args[2], STRLEN);
      validate_path(args[3], 1);
      strncpy(flags->outpath, args[3], STRLEN);
      break;
    case 'd':
      flags->disassemble = 1;
      if (argv != 4) {
        printf("Disassembly requires 3 command line arguments.\n\n");
        print_help();
        exit(-1);
      }
      validate_path(args[2], 0);
      strncpy(flags->inpath, args[2], STRLEN);
      validate_path(args[3], 1);
      strncpy(flags->outpath, args[3], STRLEN);
      break;
    case 'r':
      flags->run = 1;
      if (argv != 3) {
        printf("Disassembly requires 2 command line arguments.\n\n");
        print_help();
        exit(-1);
      }
      validate_path(args[2], 0);
      strncpy(flags->inpath, args[2], STRLEN);
      break;
    default:
      printf("Invalid argument: %s\n\n", args[1]);
      print_help();
      exit(-1);
  }

  return flags;
}

void validate_path(const char* path, int create) {
  if (!create && !(access(path, F_OK) == 0)) {
    printf("File provided: \"%s\" does not exist.\n\n", path);
    print_help();
    exit(-1);
  }
}

void print_help() {
  printf(
      "Usage: ./lc3vm [flag] (in path) (out path)\n"
      "Options:\n"
      "\t-h: Prints this (the help message)\n"
      "\t-a [in path] [out path]: Assembles your LC-3 instructions\n"
      "\t-d [in path] [out path]: Disassembles your assembled LC-3\n"
      "\t-r [in path]: Runs your assembled LC-3 binary\n");
}
