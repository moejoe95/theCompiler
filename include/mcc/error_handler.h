#ifndef MCC_ERROR_HANDLER_H
#define MCC_ERROR_HANDLER_H

#include "mcc/ast.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>


enum semantic_error_type { //todo other names?
	MCC_SC_ERROR_NO_MAIN,
	MCC_SC_ERROR_DUPLICATE_DECLARATION,
	MCC_SC_ERROR_DUPLICATE_FUNCTION_DEFINITION,
	MCC_SC_ERROR_NO_RETURN,
	MCC_SC_FUNCTION_NOT_DECLARED,
	MCC_SC_UNDEFINED_IDENTIFIER,
};

struct mcc_semantic_error { //todo extend for more information about error...for instance what variable etc...union with structs
	enum semantic_error_type error_type;
	struct mcc_ast_source_location *sloc;

	union {
		/* MCC_SC_ERROR_NO_MAIN */
		/* MCC_SC_ERROR_NO_RETURN */
		struct mcc_ast_identifier *identifier;
	};
};

struct mcc_semantic_error *get_mcc_semantic_error_struct(enum semantic_error_type error_type);

void print_lexer_error(char* filename, int last_line, int last_column, const char *msg);
void print_scanner_error(char* filename, int last_line, int last_column, char error_char);
void print_semantic_error(struct mcc_semantic_error *semantic_error);

#endif // MCC_ERROR_HANDLER_H