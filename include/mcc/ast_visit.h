// AST Visitor Infrastructure
//
// This module defines a visitor utility for traversing the AST.
//
// Instantiate the `mcc_ast_visitor` struct with the desired configuration and
// callbacks. Use this instance with the functions declared below. Each
// callback is optional, just set it to NULL.

#ifndef MCC_AST_VISIT_H
#define MCC_AST_VISIT_H

#include "mcc/ast.h"

enum mcc_ast_visit_traversal { MCC_AST_VISIT_DEPTH_FIRST };

enum mcc_ast_visit_order {
	MCC_AST_VISIT_PRE_ORDER,
	MCC_AST_VISIT_POST_ORDER,
};

// Callbacks
typedef void (*mcc_ast_visit_program_cb)(struct mcc_ast_program *, void *userdata);
typedef void (*mcc_ast_visit_function_cb)(struct mcc_ast_func_definition *, void *userdata);
typedef void (*mcc_ast_visit_statement_cb)(struct mcc_ast_statement *, void *userdata);
typedef void (*mcc_ast_visit_expression_cb)(struct mcc_ast_expression *, void *userdata);
typedef void (*mcc_ast_visit_declare_assign_cb)(struct mcc_ast_declare_assign *, void *userdata);
typedef void (*mcc_ast_visit_literal_cb)(struct mcc_ast_literal *, void *userdata);

struct mcc_ast_visitor {
	enum mcc_ast_visit_traversal traversal;
	enum mcc_ast_visit_order order;

	// This will be passed to every callback along with the corresponding AST
	// node. Use it to share data while traversing the tree.
	void *userdata;

	mcc_ast_visit_program_cb program;

	mcc_ast_visit_function_cb function;
	mcc_ast_visit_function_cb function_identifier;
	mcc_ast_visit_function_cb function_compound;

	mcc_ast_visit_statement_cb statement;
	mcc_ast_visit_statement_cb statement_expression;
	mcc_ast_visit_statement_cb statement_assignment;
	mcc_ast_visit_statement_cb statement_declaration;
	mcc_ast_visit_statement_cb statement_return;
	mcc_ast_visit_statement_cb statement_if;
	mcc_ast_visit_statement_cb statement_while;
	mcc_ast_visit_statement_cb statement_while_cond;
	mcc_ast_visit_statement_cb statement_while_stat;
	mcc_ast_visit_statement_cb statement_compound;
	mcc_ast_visit_statement_cb statement_compound_post;

	mcc_ast_visit_expression_cb expression;
	mcc_ast_visit_expression_cb expression_literal;
	mcc_ast_visit_expression_cb expression_binary_op;
	mcc_ast_visit_expression_cb expression_unary_op;
	mcc_ast_visit_expression_cb expression_parenth;
	mcc_ast_visit_expression_cb expression_identifier;
	mcc_ast_visit_expression_cb expression_array_access;
	mcc_ast_visit_expression_cb expression_call;
	mcc_ast_visit_expression_cb expression_argument;

	mcc_ast_visit_literal_cb literal;
	mcc_ast_visit_literal_cb literal_int;
	mcc_ast_visit_literal_cb literal_float;
	mcc_ast_visit_literal_cb literal_bool;
	mcc_ast_visit_literal_cb literal_string;

	mcc_ast_visit_declare_assign_cb declaration;
	mcc_ast_visit_declare_assign_cb assignment;
};

void mcc_ast_visit_program(struct mcc_ast_program *program, struct mcc_ast_visitor *visitor);

void mcc_ast_visit_function(struct mcc_ast_func_definition *function, struct mcc_ast_visitor *visitor);

void mcc_ast_visit_statement(struct mcc_ast_statement *statement, struct mcc_ast_visitor *visitor);

void mcc_ast_visit_expression(struct mcc_ast_expression *expression, struct mcc_ast_visitor *visitor);

void mcc_ast_visit_literal(struct mcc_ast_literal *literal, struct mcc_ast_visitor *visitor);

void mcc_ast_visit_declare_assign(struct mcc_ast_declare_assign *dec, struct mcc_ast_visitor *visitor);

// clang-format off

#define mcc_ast_visit(x, visitor) _Generic((x), \
		struct mcc_ast_expression *: mcc_ast_visit_expression, \
		struct mcc_ast_program *: mcc_ast_visit_program, \
		struct mcc_ast_func_definition *: mcc_ast_visit_function, \
		struct mcc_ast_statement *: mcc_ast_visit_statement, \
		struct mcc_ast_literal *:    mcc_ast_visit_literal, \
		struct mcc_ast_declare_assign *:    mcc_ast_visit_declare_assign \
	)(x, visitor)

// clang-format on

#endif // MCC_AST_VISIT_H
