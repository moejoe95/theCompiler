#include "mcc/print_ir.h"

void print_table_legend(){
    FILE *out = stdout;

    fprintf(out, "%s\t|   %s   |\t%s\t|\t%s\n", "index", "operation", "arg1", "arg2");
    fprintf(out, "----------------------------------------------------------------\n");
}

void print_table(enum ir_table_operation_type type, int index, char *arg1, char *arg2, struct mcc_ast_expression *expression){
    FILE *out = stdout;

    if(!arg1)
        arg1 = "-";
    if(!arg2)
        arg2 = "-";

    char *operation;
    switch (type) {
        case MCC_IR_TABLE_UNARY_OP:
            operation = "un";//get_un_op_string(expression->u_op); //TODO
            break;

        case MCC_IR_TABLE_BINARY_OP:
            operation = "bin";//get_bin_op_string(expression->op); //TODO
            break;

        case MCC_IR_TABLE_ASSIGNMENT:
            operation = "assign";
            break;

        case MCC_IR_TABLE_JUMPFALSE:
            operation = "jumpfalse";
            break;

        default:
            operation = "-";
            break;
    }


    fprintf(out, "%d\t|\t%s\t|\t%s\t|\t%s\n", index, operation, arg1, arg2);
}