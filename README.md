# Logan's LC-3 Virtual Machine
- Works through emulation
- Maybe when I'm cracked I'll add a JIT

# To-Do
## Assembler
- Tokenizer
    - Convert each non-empty, non-comment line into `line`, which is a union of `operation` and `directive`.
    - `operation` is going to a union of `arithmetic`, `data_move`, and `control_flow`
    - `directive` is just a regular struct
    - All of these structs will include an address field
    - Create a map of labels to addresses (symbol table)
- Line parser
    - Convert all label operands to addresses
    - Confirm that each line is valid
- Encoding + Writing
    - For each line, convert to computer language and write to the output file
    - Write the symbol table as well

Features that I need to support
- Labels
- Comments
- Case insensitivity
- Indefinite whitespace
- All opcodes
    - `ADD`
    - `AND`
    - `NOT`
    - `BR` or `BRnzp`
        - `BRz`
        - `BRp`
        - `BRn`
        - `BRzp`
        - `BRnp`
        - `BRnz`
    - `JMP`
    - `JSR`
    - `JSRR`
    - `JMP`
    - `LD`
    - `LDI`
    - `LDR`
    - `LEA`
    - `ST`
    - `STI`
    - `STR`
    - `TRAP`
    - `RET`
    - `RTI`
- Assembler Directives
    - `.ORIG`
    - `.END`
    - `.FILL`
    - `.BLKW`
    - `.STRINGZ`
- Operand types
    - Registers (`R0` through `R7`)
    - I-values
        - Decimal: `#n` (for example, `#10` or `#-1`)
        - Hex: `0xffff` (2's complement)
    - Labels as operands
    - String literals (only used in `.STRINGZ`)
## Disassembler
- Inverse of the assmbler
## VM
- Don't even talk to me

# Lint
`clang-format -i --style="{BasedOnStyle: llvm, PointerAlignment: Right}" src/**/*.c src/**/*.h`
