#ifndef MCC_PRINT_TYPE_LOG_H
#define MCC_PRINT_TYPE_LOG_H

#include "mcc/type_checking.h"

void mcc_print_type_log_header(FILE *out);
void mcc_print_type_log_line(FILE *out, struct mcc_ast_source_location *sloc, char *expr, char *type, char *status);
void mcc_print_type_log_footer(FILE *out);

#endif