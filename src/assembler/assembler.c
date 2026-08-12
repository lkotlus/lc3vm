#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "assembler/line.h"
#include "assembler/token.h"
#include "assembler/assembler.h"

static LabelMap* _label_map_factory(char* label, uint16_t addr);

void assemble(const char* fpath) {
    FILE* file = fopen(fpath, "r");
    LabelList labell = {NULL, NULL};
    
    for (Line* line = parse_line(file, -1); line; line = parse_line(file, (int)(line->addr+1))) {
        printf("0x%x\t\t", line->addr);

        if (line->label[0]) {
            printf("%s ", line->label);
            if (!labell.head) {
                labell.head = _label_map_factory(line->label, line->addr);
                labell.current = labell.head;
            }
        } 

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
    };

    fclose(file);
}

static LabelMap* _label_map_factory(char* label, uint16_t addr) {
    LabelMap* lmap = (LabelMap*)malloc(sizeof(LabelMap));

    lmap->addr = addr;
    strncpy(lmap->label, label, STRLEN-1);
    lmap->label[STRLEN-1] = '\0';
    lmap->next = NULL;

    return lmap;
}
