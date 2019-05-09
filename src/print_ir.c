#include "mcc/print_ir.h"
#include <string.h>

void print_table_legend(FILE *out){
    fprintf(out, "%s\t|   %s   \t|\t%s\t|\t%s\n", "index", "operation", "arg1", "arg2");
    fprintf(out, "------------------------------------------------------------\n");
}

void print_line(struct mcc_ir_table *table, FILE *out){

    if(!table->arg2){
        struct mcc_ir_entity *entity2 = create_new_ir_entity();
        sprintf(entity2->lit,"-");
        table->arg2 = entity2;
    }

    char operation[32] = {0};
    switch (table->op_type) {
        case MCC_IR_TABLE_UNARY_OP:
            sprintf(operation, get_un_op_string(table->un_op));
            strcat(operation, "\t");
            break;

        case MCC_IR_TABLE_BINARY_OP:
            sprintf(operation, get_bin_op_string(table->bin_op));
            strcat(operation, "\t");
            break;

        case MCC_IR_TABLE_ASSIGNMENT:
            sprintf(operation, "assign\t");
            break;

        case MCC_IR_TABLE_JUMPFALSE:
            sprintf(operation, "jumpfalse");
            break;

        case MCC_IR_TABLE_JUMP:
            sprintf(operation, "jump\t");
            break;

        case MCC_IR_TABLE_LABEL:
            sprintf(operation, "label\t");
            break;

        case MCC_IR_TABLE_COPY:
            sprintf(operation, "copy\t");
            break;
        default:
            sprintf(operation, "-\t");
            break;
    }


    fprintf(out, "%d\t|\t%s\t|\t%s\t|\t%s\n", table->index, operation, table->arg1->lit, table->arg2->lit);
}

void mcc_print_ir_table(struct mcc_ir_table *table, FILE *out)
{
    print_table_legend(out);
    struct mcc_ir_table *current_table = table->next_table;
    while (current_table != NULL){
        print_line(current_table, out);
        current_table = current_table->next_table;
    }
    printf("\n");
}
