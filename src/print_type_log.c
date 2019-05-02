#include "mcc/print_type_log.h"

void mcc_print_type_log_header(FILE *out)
{
    assert(out);

	fprintf(out, "trace type checking ...\n");
	fprintf(out, "\nLocation\t|\tExpression\t|\tType\t|\tStatus\n");
    fprintf(out, "----------------------------------------------------------------------\n");
}

void mcc_print_type_log_line(FILE *out, struct mcc_ast_source_location *sloc, char *expr, char *type, char *status){
    fprintf(out, "%d:%d\t\t|\t%s\t\t|\t%s\t|\t%s\n", sloc->end_line+1, sloc->end_col+1, expr, type, status);
}

void mcc_print_type_log_footer(FILE *out)
{
    assert(out);
	fprintf(out, "\n");
}