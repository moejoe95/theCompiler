#include "mcc/print_ir.h"

void print_table_legend(){
    FILE *out = stdout;

    fprintf(out, "%s\t|   %s   |\t%s\t|\t%s\n", "index", "operation", "arg1", "arg2");
    fprintf(out, "----------------------------------------------------------------\n");
}

void print_table(struct mcc_ir_table *table, int index, char *arg1, char *arg2){
    FILE *out = stdout;

    if(!arg1)
        arg1 = "-";
    if(!arg2)
        arg2 = "-";

    char *operation;
    switch (table->op_type) {
        case MCC_IR_TABLE_UNARY_OP:
            operation = get_un_op_string(table->un_op);
            break;

        case MCC_IR_TABLE_BINARY_OP:
            operation = get_bin_op_string(table->bin_op);
            break;

        case MCC_IR_TABLE_ASSIGNMENT:
            operation = "assign";
            break;

        case MCC_IR_TABLE_JUMPFALSE:
            operation = "jumpfalse";
            break;

        case MCC_IR_TABLE_JUMP:
            operation = "jump";
            break;

        case MCC_IR_TABLE_LABEL:
            operation = "label";
            break;

        case MCC_IR_TABLE_COPY:
            operation = "copy";
            break;
        default:
            operation = "-";
            break;
    }


    fprintf(out, "%d\t|\t%s\t|\t%s\t|\t%s\n", index, operation, arg1, arg2);
}