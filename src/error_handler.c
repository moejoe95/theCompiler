#include "mcc/error_handler.h"

void print_lexer_error(char *filename, int last_line, int last_column, const char *msg, FILE* out)
{
	if (out == stdout) {
		out = stderr;
	}
	fprintf(out, "%s:%d:%d: error: %s \n", filename, last_line + 1 , last_column, msg);
}

void print_scanner_error(char *filename, int last_line, int last_column, char error_char, FILE* out)
{ 	
	if (out == stdout) {
		out = stderr;
	}
	fprintf(out, "%s:%d:%d: unknown character '%c'\n", filename, last_line + 1, last_column, error_char);
}

void print_semantic_error(struct mcc_semantic_error *semantic_error, FILE *out)
{
	if (out == stdout) {
		out = stderr;
	}

	enum mcc_ast_type lhs_type;
	enum mcc_ast_type rhs_type;

	switch (semantic_error->error_type) {

	case MCC_SC_ERROR_NO_MAIN:
		fprintf(out, "%s:%d:%d: error: %s \n", semantic_error->sloc->filename,
		        semantic_error->sloc->end_line + 1, semantic_error->sloc->end_col + 1,
		        "no 'main' function found");
		break;

	case MCC_SC_ERROR_DUPLICATE_DECLARATION:
		assert(semantic_error->identifier->name);
		fprintf(out, "%s:%d:%d: error: %s (variable '%s')\n", semantic_error->sloc->filename,
		        semantic_error->sloc->end_line + 1, semantic_error->sloc->end_col + 1, "duplicate declaration",
		        semantic_error->identifier->name);
		break;

	case MCC_SC_ERROR_DUPLICATE_FUNCTION_DEFINITION:
		assert(semantic_error->identifier->name);
		fprintf(out, "%s:%d:%d: error: %s (id '%s')\n", semantic_error->sloc->filename,
		        semantic_error->sloc->end_line + 1, semantic_error->sloc->end_col + 1,
		        "duplicate function definition", semantic_error->identifier->name);
		break;

	case MCC_SC_ERROR_NO_RETURN:
		assert(semantic_error->identifier->name);
		fprintf(out, "%s:%d:%d: error: %s (func '%s')\n", semantic_error->sloc->filename,
		        semantic_error->sloc->end_line + 1, semantic_error->sloc->end_col + 1,
		        "no return value in non void function", semantic_error->identifier->name);
		break;

	case MCC_SC_ERROR_FUNCTION_NOT_DECLARED:
		assert(semantic_error->identifier->name);
		fprintf(out, "%s:%d:%d: error: %s (func '%s')\n", semantic_error->sloc->filename,
		        semantic_error->sloc->end_line + 1, semantic_error->sloc->end_col + 1, "function not declared",
		        semantic_error->identifier->name);
		break;

	case MCC_SC_ERROR_UNDEFINED_IDENTIFIER:
		assert(semantic_error->identifier->name);
		fprintf(out, "%s:%d:%d: error: %s (id '%s')\n", semantic_error->sloc->filename,
		        semantic_error->sloc->end_line + 1, semantic_error->sloc->end_col + 1, "undefined identifier",
		        semantic_error->identifier->name);
		break;
	case MCC_SC_ERROR_INVALID_ASSIGNMENT:
		assert(semantic_error);
		fprintf(out, "%s:%d:%d: error: %s (expected type '%s', but got type '%s')\n",
		        semantic_error->sloc->filename, semantic_error->sloc->end_line + 1,
		        semantic_error->sloc->end_col + 1, "invalid assignment",
		        get_type_string(semantic_error->lhs_type), get_type_string(semantic_error->rhs_type));
		break;
	case MCC_SC_ERROR_INVALID_RETURN_TYPE:
		fprintf(out, "%s:%d:%d: error: %s (expected type '%s', but got type '%s')\n",
		        semantic_error->sloc->filename, semantic_error->sloc->end_line + 1,
		        semantic_error->sloc->end_col + 1, "invalid return type",
		        get_type_string(semantic_error->func_type), get_type_string(semantic_error->ret_type));
		break;
	case MCC_SC_ERROR_INVALID_AR_OPERATION:
		fprintf(out, "%s:%d:%d: error: %s (type '%s')\n", semantic_error->sloc->filename,
		        semantic_error->sloc->end_line + 1, semantic_error->sloc->end_col + 1,
		        "arithmetic operations not allowed on type", get_type_string(semantic_error->ret_type));
		break;
	case MCC_SC_ERROR_INVALID_LOG_OPERATION:
		fprintf(out, "%s:%d:%d: error: %s (type '%s')\n", semantic_error->sloc->filename,
		        semantic_error->sloc->end_line + 1, semantic_error->sloc->end_col + 1,
		        "logical operations not allowed on type", get_type_string(semantic_error->ret_type));
		break;
	case MCC_SC_ERROR_INVALID_BIN_OPERATION:
		lhs_type = semantic_error->bin_expr->lhs->expression_type;
		rhs_type = semantic_error->bin_expr->rhs->expression_type;
		if (semantic_error->bin_expr->lhs->type == MCC_AST_EXPRESSION_TYPE_ARRAY_ACCESS)
			lhs_type = semantic_error->bin_expr->lhs->array_access_id->expression_type;
		if (semantic_error->bin_expr->rhs->type == MCC_AST_EXPRESSION_TYPE_ARRAY_ACCESS)
			rhs_type = semantic_error->bin_expr->rhs->array_access_id->expression_type;

		fprintf(out, "%s:%d:%d: error: operation '%s' not allowed on types ('%s' and '%s')\n",
		        semantic_error->sloc->filename, semantic_error->sloc->end_line + 1,
		        semantic_error->sloc->end_col + 1, get_bin_op_string(semantic_error->bin_expr->op),
		        get_type_string(lhs_type), get_type_string(rhs_type));
		break;
	case MCC_SC_ERROR_LITERAL_VOID:
		assert(semantic_error->identifier->name);
		fprintf(out, "%s:%d:%d: error: %s (id '%s')\n", semantic_error->sloc->filename,
		        semantic_error->sloc->end_line + 1, semantic_error->sloc->end_col + 1,
		        "literal cannot be type void", semantic_error->identifier->name);
		break;
	case MCC_SC_ERROR_NUMBER_ARGUMENTS:
		assert(semantic_error->sloc);
		assert(semantic_error->identifier->name);
		fprintf(out, "%s:%d:%d: error: %s (function '%s' expected '%d' got '%d')\n",
		        semantic_error->sloc->filename, semantic_error->sloc->end_line + 1,
		        semantic_error->sloc->end_col + 1, "wrong number of arguments for function call",
		        semantic_error->identifier->name, semantic_error->expArgs, semantic_error->gotArgs);
		break;
	case MCC_SC_ERROR_INVALID_CONDITION_TYPE:
		assert(semantic_error->sloc);
		assert(semantic_error->expr_type);
		fprintf(out, "%s:%d:%d: error: %s (type '%s')\n", semantic_error->sloc->filename,
		        semantic_error->sloc->end_line + 1, semantic_error->sloc->end_col + 1,
		        "type not allowed in evaluation condition", get_type_string(semantic_error->expr_type));
		break;
	case MCC_SC_ERROR_INVALID_CONDITION_BIN_EXPR:
		assert(semantic_error->sloc);
		assert(semantic_error->bin_expr);
		fprintf(out, "%s:%d:%d: error: %s (operation '%s')\n", semantic_error->sloc->filename,
		        semantic_error->sloc->end_line + 1, semantic_error->sloc->end_col + 1,
		        "binary operation not allowed in evaluation condition",
		        get_bin_op_string(semantic_error->bin_expr->op));
		break;
	case MCC_SC_ERROR_INVALID_CONDITION_UN_EXPR:
		assert(semantic_error->sloc);
		assert(semantic_error->un_expr);
		fprintf(out, "%s:%d:%d: error: %s (operation '%s')\n", semantic_error->sloc->filename,
		        semantic_error->sloc->end_line + 1, semantic_error->sloc->end_col + 1,
		        "unary operation not allowed in evaluation condition",
		        get_un_op_string(semantic_error->un_expr->op));
		break;
	case MCC_SC_ERROR_TYPE_NO_CONDITION:
		assert(semantic_error->sloc);
		fprintf(out, "%s:%d:%d: error: %s (operation '%s')\n", semantic_error->sloc->filename,
		        semantic_error->sloc->end_line + 1, semantic_error->sloc->end_col + 1,
		        "missing evaluation condition");
		break;
	case MCC_SC_ERROR_TYPE_INVALID_ARRAY_ACCESS:
		assert(semantic_error->sloc);
		fprintf(out, "%s:%d:%d: error: %s (expected type 'INT', but got type '%s')\n",
		        semantic_error->sloc->filename, semantic_error->sloc->end_line + 1,
		        semantic_error->sloc->end_col + 1, "invalid array access",
		        get_type_string(semantic_error->lhs_type));
		break;
	case MCC_SC_ERROR_TYPE_INVALID_ARGUMENT_TYPE:
		assert(semantic_error->sloc);
		assert(semantic_error->par_type);
		fprintf(out, "%s:%d:%d: error: %s (expected type '%s', but got type '%s')\n",
		        semantic_error->sloc->filename, semantic_error->sloc->end_line + 1,
		        semantic_error->sloc->end_col + 1, "invalid argument type",
		        get_type_string(semantic_error->par_type), get_type_string(semantic_error->arg_type));
		break;
	}
}

struct mcc_semantic_error *get_mcc_semantic_error_struct(enum semantic_error_type error_type)
{
	struct mcc_semantic_error *semantic_error = malloc(sizeof(*semantic_error));
	if (!semantic_error) {
		return NULL;
	}
	semantic_error->error_type = error_type;

	return semantic_error;
}
