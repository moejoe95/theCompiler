#ifndef MCC_ERROR_HANDLER_H
#define MCC_ERROR_HANDLER_H

#include "mcc/ast.h"
#include "mcc/symbol_table.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

enum semantic_error_type {
	MCC_SC_ERROR_NO_MAIN,
	MCC_SC_ERROR_DUPLICATE_DECLARATION,
	MCC_SC_ERROR_DUPLICATE_FUNCTION_DEFINITION,
	MCC_SC_ERROR_NO_RETURN,
	MCC_SC_ERROR_FUNCTION_NOT_DECLARED,
	MCC_SC_ERROR_UNDEFINED_IDENTIFIER,
	MCC_SC_ERROR_INVALID_ASSIGNMENT,
	MCC_SC_ERROR_INVALID_ASSIGNMENT_ARR,
	MCC_SC_ERROR_INVALID_RETURN_TYPE,
	MCC_SC_ERROR_INVALID_AR_OPERATION,
	MCC_SC_ERROR_INVALID_LOG_OPERATION,
	MCC_SC_ERROR_INVALID_BIN_OPERATION,
	MCC_SC_ERROR_LITERAL_VOID,
	MCC_SC_ERROR_NUMBER_ARGUMENTS,
	MCC_SC_ERROR_INVALID_CONDITION_TYPE,
	MCC_SC_ERROR_INVALID_CONDITION_BIN_EXPR,
	MCC_SC_ERROR_INVALID_CONDITION_UN_EXPR,
	MCC_SC_ERROR_TYPE_NO_CONDITION,
	MCC_SC_ERROR_TYPE_INVALID_ARRAY_ACCESS,
	MCC_SC_ERROR_TYPE_INVALID_ARGUMENT_TYPE
};

struct mcc_semantic_error {
	enum semantic_error_type error_type;
	struct mcc_ast_source_location *sloc;

	union {
		struct mcc_ast_identifier *identifier;

		struct {
			struct mcc_symbol *symbol;
			struct mcc_ast_expression *rhs;
		};

		struct {
			enum mcc_ast_type ret_type;
			enum mcc_ast_type func_type;
		};

		enum mcc_ast_type expr_type;

		struct mcc_ast_expression *bin_expr;
		struct mcc_ast_expression *un_expr;

		struct {
			struct mcc_ast_identifier *func_identifier;
			int expArgs;
			int gotArgs;
		};

		struct {
			enum mcc_ast_type rhs_type;
			enum mcc_ast_type lhs_type;
		};

		struct {
			enum mcc_ast_type par_type;
			enum mcc_ast_type arg_type;
		};
	};
};

struct mcc_semantic_error *get_mcc_semantic_error_struct(enum semantic_error_type error_type);

void print_lexer_error(char *filename, int last_line, int last_column, const char *msg, FILE *out);
void print_scanner_error(char *filename, int last_line, int last_column, char error_char, FILE *out);
void print_semantic_error(struct mcc_semantic_error *semantic_error, FILE *out);

#endif // MCC_ERROR_HANDLER_H