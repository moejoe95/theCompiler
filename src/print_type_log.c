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

void print_line(FILE *out)
{
	for (int i = 0; i < 72; i++)
		fprintf(out, "-");
	fprintf(out, "\n");
}

void mcc_print_type_log_header(FILE *out)
{
	assert(out);
	fprintf(out, "type checking ...\n\n");
	print_line(out);
	fprintf(out, "| Location | %-25.25s | Type-LHS - Type-RHS | Status  |\n", "Expression");
	print_line(out);
}

void mcc_print_type_log_footer(FILE *out)
{
	print_line(out);
}

void mcc_print_type_log_assign(FILE *out, struct mcc_type_log *log, char *expr)
{
	char tmp[19] = {0};
	sprintf(tmp, "%s <- %s", get_type_string(log->lhs_type), get_type_string(log->rhs_type));
	char nmb[8] = {0};
	sprintf(nmb, "%d:%d", log->sloc->end_line, log->sloc->end_col);
	fprintf(out, "| %-8.8s | %-25.25s | %-19.19s | %-7.7s |\n", nmb, expr, tmp, get_status_string(log->status));
}

void mcc_print_type_log_return(FILE *out, struct mcc_type_log *log, char *expr)
{
	char tmp[19] = {0};
	sprintf(tmp, "%s ~ %s", get_type_string(log->lhs_type), get_type_string(log->rhs_type));
	char nmb[8] = {0};
	sprintf(nmb, "%d:%d", log->sloc->end_line, log->sloc->end_col);
	fprintf(out, "| %-8.8s | %-25.25s | %-19.19s | %-7.7s |\n", nmb, expr, tmp, get_status_string(log->status));
}

void mcc_print_type_log_bin(FILE *out, struct mcc_type_log *log, char *expr)
{
	char tmp[19] = {0};
	sprintf(tmp, "%s ~ %s", get_type_string(log->lhs_type), get_type_string(log->rhs_type));
	char nmb[8] = {0};
	sprintf(nmb, "%d:%d", log->sloc->end_line, log->sloc->end_col);
	fprintf(out, "| %-8.8s | %-25.25s | %-19.19s | %-7.7s |\n", nmb, expr, tmp, get_status_string(log->status));
}

void mcc_print_type_log_op(FILE *out, struct mcc_type_log *log, char *expr)
{
	char tmp[19] = {0};
	sprintf(tmp, "%s op %s", get_type_string(log->lhs_type), get_type_string(log->rhs_type));
	char nmb[8] = {0};
	sprintf(nmb, "%d:%d", log->sloc->end_line, log->sloc->end_col);
	fprintf(out, "| %-8.8s | %-25.25s | %-19.19s | %-7.7s |\n", nmb, expr, tmp, get_status_string(log->status));
}

void mcc_print_type_log_u_op(FILE *out, struct mcc_type_log *log, char *expr)
{
	char tmp[19] = {0};
	sprintf(tmp, "op %s", get_type_string(log->lhs_type));
	char nmb[8] = {0};
	sprintf(nmb, "%d:%d", log->sloc->end_line, log->sloc->end_col);
	fprintf(out, "| %-8.8s | %-25.25s | %-19.19s | %-7.7s |\n", nmb, expr, tmp, get_status_string(log->status));
}

void mcc_print_type_log_decl(FILE *out, struct mcc_type_log *log, char *expr)
{
	char nmb[8] = {0};
	sprintf(nmb, "%d:%d", log->sloc->end_line, log->sloc->end_col);
	fprintf(out, "| %-8.8s | %-25.25s | %-19.19s | %-7.7s |\n", nmb, expr, get_type_string(log->lhs_type),
	        get_status_string(log->status));
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