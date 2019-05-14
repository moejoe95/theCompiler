#include "mcc/print_type_log.h"
#include "mcc/error_handler.h"

char *get_status_string(enum mcc_type_status status)
{
	switch (status) {
	case MCC_TYPE_VALID:
		return "VALID";
	case MCC_TYPE_INVALID:
		return "INVALID";
	}

	return "";
}

void mcc_print_type_log_header(FILE *out)
{
	assert(out);
	fprintf(out, "Location\t|\tExpression\t|\tType-LHS - Type-RHS\t|\tStatus\n");
	fprintf(out, "--------------------------------------------------------------------------------------\n");
}

void mcc_print_type_log_assign(FILE *out, struct mcc_type_log *log, char *expr)
{
	fprintf(out, "%d:%d\t\t|\t%s\t|\t%s <- %s\t\t|\t%s\n", log->sloc->end_line, log->sloc->end_col, expr,
	        get_type_string(log->lhs_type), get_type_string(log->rhs_type), get_status_string(log->status));
}

void mcc_print_type_log_return(FILE *out, struct mcc_type_log *log, char *expr)
{
	fprintf(out, "%d:%d\t\t|\t%s\t\t|\t%s ~ %s\t\t|\t%s\n", log->sloc->end_line, log->sloc->end_col, expr,
	        get_type_string(log->lhs_type), get_type_string(log->rhs_type), get_status_string(log->status));
}

void mcc_print_type_log_bin(FILE *out, struct mcc_type_log *log, char *expr)
{
	fprintf(out, "%d:%d\t\t|\t%s\t\t|\t%s ~ %s\t\t|\t%s\n", log->sloc->end_line, log->sloc->end_col, expr,
	        get_type_string(log->lhs_type), get_type_string(log->rhs_type), get_status_string(log->status));
}

void mcc_print_type_log_op(FILE *out, struct mcc_type_log *log, char *expr)
{
	fprintf(out, "%d:%d\t\t|\t%s\t\t|\t%s op %s\t\t|\t%s\n", log->sloc->end_line, log->sloc->end_col, expr,
	        get_type_string(log->lhs_type), get_type_string(log->rhs_type), get_status_string(log->status));
}

void mcc_print_type_log_u_op(FILE *out, struct mcc_type_log *log, char *expr)
{
	fprintf(out, "%d:%d\t\t|\t%s\t\t|\top %s\t\t|\t%s\n", log->sloc->end_line, log->sloc->end_col, expr,
	        get_type_string(log->lhs_type), get_status_string(log->status));
}

void mcc_print_type_log_decl(FILE *out, struct mcc_type_log *log, char *expr)
{
	fprintf(out, "%d:%d\t\t|\t%s\t|\t%s\t\t\t|\t%s\n", log->sloc->end_line, log->sloc->end_col, expr,
	        get_type_string(log->lhs_type), get_status_string(log->status));
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