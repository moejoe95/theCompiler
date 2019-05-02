#include "mcc/print_type_log.h"
#include "mcc/error_handler.h"

void mcc_print_type_log_header(FILE *out)
{
    assert(out);

	fprintf(out, "trace type checking ...\n");
	fprintf(out, "\nLocation\t|\tExpression\t|\tType-LHS - Type-RHS\t|\tStatus\n");
    fprintf(out, "--------------------------------------------------------------------------------------\n");
}

void mcc_print_type_log(FILE *out, struct mcc_type_log *log, char *expr){
    fprintf(out, "%d:%d\t\t|\t%s\t\t|\t%s <- %s\t\t|\t%s\n", 
        log->error->sloc->end_line+1, 
        log->error->sloc->end_col+1,
        expr, 
        get_type_string(log->error->lhs_type), 
        get_type_string(log->error->rhs_type), 
        get_status_string(log->status)
        );
}

void mcc_print_type_log_op(FILE *out, struct mcc_type_log *log, char *expr){
    fprintf(out, "%d:%d\t\t|\t%s\t\t|\t%s op %s\t\t|\t%s\n", 
        log->error->sloc->end_line+1, 
        log->error->sloc->end_col+1,
        expr, 
        get_type_string(log->error->lhs_type), 
        get_type_string(log->error->rhs_type), 
        get_status_string(log->status)
        );
}

void mcc_print_type_log_footer(FILE *out)
{
    assert(out);
	fprintf(out, "\n");
}

struct mcc_type_log *get_mcc_type_log_struct(enum mcc_type_status status)
{
	struct mcc_type_log *type_log = malloc(sizeof(*type_log));
	if (!type_log) {
		return NULL;
	}
	type_log->status = status;

	return type_log;
}