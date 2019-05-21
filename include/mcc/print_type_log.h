#ifndef MCC_PRINT_TYPE_LOG_H
#define MCC_PRINT_TYPE_LOG_H

#include "mcc/error_handler.h"
#include "mcc/type_checking.h"

enum mcc_type_status { MCC_TYPE_VALID, MCC_TYPE_INVALID };

struct mcc_type_log {
	struct mcc_ast_source_location *sloc;
	enum mcc_ast_type lhs_type;
	enum mcc_ast_type rhs_type;
	enum mcc_type_status status;
};

void mcc_print_type_log_header(FILE *out);
void mcc_print_type_log_assign(FILE *out, struct mcc_type_log *log, char *expression);
void mcc_print_type_log_return(FILE *out, struct mcc_type_log *log, char *expression);
void mcc_print_type_log_op(FILE *out, struct mcc_type_log *log, char *expression);
void mcc_print_type_log_bin(FILE *out, struct mcc_type_log *log, char *expression);
void mcc_print_type_log_u_op(FILE *out, struct mcc_type_log *log, char *expr);
void mcc_print_type_log_u_bin(FILE *out, struct mcc_type_log *log, char *expr);
void mcc_print_type_log_decl(FILE *out, struct mcc_type_log *log, char *expr);
struct mcc_type_log *get_mcc_type_log_struct(enum mcc_type_status status);
void mcc_print_type_log_footer(FILE *out);
char *get_status_string(enum mcc_type_status status);
void mcc_delete_log_struct(struct mcc_type_log *log);
void print_line(FILE *out);

#endif