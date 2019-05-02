#ifndef MCC_PRINT_TYPE_LOG_H
#define MCC_PRINT_TYPE_LOG_H

#include "mcc/type_checking.h"
#include "mcc/error_handler.h"

enum mcc_type_status { MCC_TYPE_VALID, MCC_TYPE_INVALID};

struct mcc_type_log {
    struct mcc_semantic_error *error;
    enum mcc_type_status status;
};

void mcc_print_type_log_header(FILE *out);
void mcc_print_type_log(FILE *out, struct mcc_type_log *log, char *expression);
void mcc_print_type_log_op(FILE *out, struct mcc_type_log *log, char *expression);
struct mcc_type_log *get_mcc_type_log_struct(enum mcc_type_status status);
void mcc_print_type_log_footer(FILE *out);


static char *get_status_string(enum mcc_type_status status)
{
	switch (status) {
	case MCC_TYPE_VALID:
		return "VALID";
	case MCC_TYPE_INVALID:
		return "INVALID";
	}

	return "";
}

#endif