#include <stdlib.h>
#include <stdio.h>
#include "assembler/line.h"
#include "assembler/token.h"

void assemble(const char* fpath) {
    FILE* file = fopen(fpath, "r");
    
    for (Line* line = parse_line(file); line; line = parse_line(file)) {
        if (line->label[0]) printf("%s ", line->label);

        if (line->type == LINE_OPERATION) {
            printf("%d ", line->line.operation.opcode);

            for (int i = 0; i < line->line.operation.n_ops; i++) {
                switch (line->line.operation.operands[i].type) {
                    case OPT_REG: 
                        printf("%d", line->line.operation.operands[i].operand.reg);
                        break;
                    case OPT_LAB:
                        printf("%s", line->line.operation.operands[i].operand.label);
                        break;
                    case OPT_IVA:
                        printf("%d", line->line.operation.operands[i].operand.ival);
                        break;
                    case OPT_STR: break;
                    case OPERAND_INVALID: printf("OPERAND INVALID\n"); break;
                }

                if (i != line->line.operation.n_ops - 1) printf(",");
                printf(" ");
            }
        }
        else {
            printf("%d ", line->line.directive.type);

            switch (line->line.directive.has_operand) {
                case OPT_REG: 
                    printf("%d", line->line.directive.operand.operand.reg);
                    break;
                case OPT_LAB:
                    printf("%s", line->line.directive.operand.operand.label);
                    break;
                case OPT_IVA:
                    printf("%d", line->line.directive.operand.operand.ival);
                    break;
                case OPT_STR:
                    printf("%s", line->line.directive.operand.operand.stringz);
                    break;
                case OPERAND_INVALID: 
                    break;
            }
        }

        printf("\n");
        free(line);
    };

    fclose(file);
}
