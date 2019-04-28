#include "mcc/error_handler.h"

void print_lexer_error(char* filename, int last_line, int last_column, const char *msg){
    fprintf(stderr, "%s:%d:%d: error: %s \n", filename, last_line+1, last_column, msg);
}

void print_scanner_error(char* filename, int last_line, int last_column, char error_char){
	fprintf(stderr, "%s:%d:%d: unknown character '%c'\n", filename, last_line+1, last_column, error_char);
}

void print_semantic_error(struct mcc_semantic_error *semantic_error){
    switch (semantic_error->error_type) {

		case MCC_SC_ERROR_NO_MAIN:
			fprintf(stderr, "%s:%d:%d: error: %s \n", 
			semantic_error->sloc->filename, semantic_error->sloc->end_line+1,
			semantic_error->sloc->end_col, "no 'main' function found");
			break;

		case MCC_SC_ERROR_DUPLICATE_DECLARATION:
			assert(semantic_error->identifier->name);
			fprintf(stderr, "%s:%d:%d: error: %s (variable '%s')\n", 
			semantic_error->sloc->filename, semantic_error->sloc->end_line+1,
			semantic_error->sloc->end_col, "duplicate declaration", semantic_error->identifier->name);
			break;

		case MCC_SC_ERROR_DUPLICATE_FUNCTION_DEFINITION:
			assert(semantic_error->identifier->name);
			fprintf(stderr, "%s:%d:%d: error: %s (id '%s')\n", 
			semantic_error->sloc->filename, semantic_error->sloc->end_line+1,
			semantic_error->sloc->end_col, "duplicate function definition", semantic_error->identifier->name);
			break;

		case MCC_SC_ERROR_NO_RETURN:
			assert(semantic_error->identifier->name);
			fprintf(stderr, "%s:%d:%d: error: %s (func '%s')\n", 
			semantic_error->sloc->filename, semantic_error->sloc->end_line+1,
			semantic_error->sloc->end_col, "no return value in non void function", semantic_error->identifier->name);
			break;

		case MCC_SC_ERROR_FUNCTION_NOT_DECLARED:
			assert(semantic_error->identifier->name);
			fprintf(stderr, "%s:%d:%d: error: %s (func '%s')\n", 
			semantic_error->sloc->filename, semantic_error->sloc->end_line+1,
			semantic_error->sloc->end_col, "function not declared", semantic_error->identifier->name);
			break;

		case MCC_SC_ERROR_UNDEFINED_IDENTIFIER:
			assert(semantic_error->identifier->name);
			fprintf(stderr, "%s:%d:%d: error: %s (id '%s')\n", 
			semantic_error->sloc->filename, semantic_error->sloc->end_line+1,
			semantic_error->sloc->end_col, "undefined identifier", semantic_error->identifier->name);
			break;
		case MCC_SC_ERROR_INVALID_ASSIGNMENT: //todo
			assert(semantic_error->identifier->name);
			fprintf(stderr, "%s:%d:%d: error: %s (expected type '%d', but got type '%d')\n", 
			semantic_error->sloc->filename, semantic_error->sloc->end_line+1,
			semantic_error->sloc->end_col, "invalid assignment", semantic_error->symbol->type, semantic_error->rhs->expression_type);
			break;
		case MCC_SC_ERROR_INVALID_RETURN_TYPE:
			fprintf(stderr, "%s:%d:%d: error: %s (expected type '%s', but got type '%s')\n", 
			semantic_error->sloc->filename, semantic_error->sloc->end_line+1,
			semantic_error->sloc->end_col, "invalid return type", 
			get_type_string(semantic_error->func_type), get_type_string(semantic_error->ret_type));
			break;
		case MCC_SC_ERROR_INVALID_AR_OPERATION:
			fprintf(stderr, "%s:%d:%d: error: %s (type '%s')\n", 
			semantic_error->sloc->filename, semantic_error->sloc->end_line+1,
			semantic_error->sloc->end_col, "arithmetic operations not allowed on type", 
			get_type_string(semantic_error->ret_type));
			break;
		case MCC_SC_ERROR_INVALID_LOG_OPERATION:
			fprintf(stderr, "%s:%d:%d: error: %s (type '%s')\n", 
			semantic_error->sloc->filename, semantic_error->sloc->end_line+1,
			semantic_error->sloc->end_col, "logical operations not allowed on type", 
			get_type_string(semantic_error->ret_type));
			break;
		case MCC_SC_ERROR_INVALID_BIN_OPERATION:
			fprintf(stderr, "%s:%d:%d: error: operation '%s' not allowed on types ('%s' and '%s')\n", 
			semantic_error->sloc->filename, semantic_error->sloc->end_line+1,
			semantic_error->sloc->end_col, get_bin_op_string(semantic_error->bin_expr->op),
			get_type_string(semantic_error->bin_expr->lhs->expression_type), 
			get_type_string(semantic_error->bin_expr->rhs->expression_type));
			break;
		case MCC_SC_ERROR_LITERAL_VOID:
			assert(semantic_error->identifier->name);
			fprintf(stderr, "%s:%d:%d: error: %s (id '%s')\n", 
			semantic_error->sloc->filename, semantic_error->sloc->end_line+1,
			semantic_error->sloc->end_col, "literal cannot be type void", semantic_error->identifier->name);
			break;
	}
}

struct mcc_semantic_error *get_mcc_semantic_error_struct(enum semantic_error_type error_type){
	struct mcc_semantic_error *semantic_error = malloc(sizeof(*semantic_error));
	if (!semantic_error) {
		return NULL;
	}
	semantic_error->error_type = error_type;

	return semantic_error;
}


