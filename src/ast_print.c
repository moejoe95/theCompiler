#include "mcc/ast_print.h"

#include <assert.h>

#include "mcc/ast_visit.h"

const char *mcc_ast_print_binary_op(enum mcc_ast_binary_op op)
{
	switch (op) {
	case MCC_AST_BINARY_OP_ADD:
		return "+";
	case MCC_AST_BINARY_OP_SUB:
		return "-";
	case MCC_AST_BINARY_OP_MUL:
		return "*";
	case MCC_AST_BINARY_OP_DIV:
		return "/";
	case MCC_AST_BINARY_OP_ST:
		return "<";
	case MCC_AST_BINARY_OP_GT:
		return ">";
	case MCC_AST_BINARY_OP_SE:
		return "<=";
	case MCC_AST_BINARY_OP_GE:
		return ">=";
	case MCC_AST_BINARY_OP_LAND:
		return "&&";
	case MCC_AST_BINARY_OP_LOR:
		return "||";
	case MCC_AST_BINARY_OP_EQ:
		return "==";
	case MCC_AST_BINARY_OP_NEQ:
		return "!=";
	}

	return "unknown op";
}

const char *mcc_ast_print_unary_op(enum mcc_ast_unary_op op)
{
	switch (op) {
	case MCC_AST_UNARY_OP_MINUS:
		return "-";
	case MCC_AST_UNARY_OP_NOT:
		return "!";
	}

	return "unknown op";
}

// ---------------------------------------------------------------- DOT Printer

#define LABEL_SIZE 64

static void print_dot_begin(FILE *out)
{
	assert(out);

	fprintf(out, "digraph \"AST\" {\n"
	             "\tnodesep=0.6\n");
}

static void print_dot_end(FILE *out)
{
	assert(out);

	fprintf(out, "}\n");
}

static void print_dot_node(FILE *out, const void *node, const char *label)
{
	assert(out);
	assert(node);
	assert(label);

	fprintf(out, "\t\"%p\" [shape=box, label=\"%s\"];\n", node, label);
}

static void print_dot_edge(FILE *out, const void *src_node, const void *dst_node, const char *label)
{
	assert(out);
	assert(src_node);
	assert(dst_node);
	assert(label);

	fprintf(out, "\t\"%p\" -> \"%p\" [label=\"%s\"];\n", src_node, dst_node, label);
}

static void print_dot_expression_literal(struct mcc_ast_expression *expression, void *data)
{
	assert(expression);
	assert(data);

	FILE *out = data;
	print_dot_node(out, expression, "expr: lit");
	print_dot_edge(out, expression, expression->literal, "literal");
}

static void print_dot_expression_binary_op(struct mcc_ast_expression *expression, void *data)
{
	assert(expression);
	assert(data);

	char label[LABEL_SIZE] = {0};
	snprintf(label, sizeof(label), "expr: %s", mcc_ast_print_binary_op(expression->op));

	FILE *out = data;
	print_dot_node(out, expression, label);
	print_dot_edge(out, expression, expression->lhs, "lhs");
	print_dot_edge(out, expression, expression->rhs, "rhs");
}

static void print_dot_expression_unary_op(struct mcc_ast_expression *expression, void *data)
{
	assert(expression);
	assert(data);

	char label[LABEL_SIZE] = {0};
	snprintf(label, sizeof(label), "expr: %s", mcc_ast_print_unary_op(expression->op));

	FILE *out = data;
	print_dot_node(out, expression, label);
	print_dot_edge(out, expression, expression->rhs, "rhs");
}

static void print_dot_expression_parenth(struct mcc_ast_expression *expression, void *data)
{
	assert(expression);
	assert(data);

	FILE *out = data;
	print_dot_node(out, expression, "( )");
	print_dot_edge(out, expression, expression->expression, "expression");
}

static void print_dot_expression_identifier(struct mcc_ast_expression *expression, void *data)
{
	assert(expression);
	assert(data);

	char label[LABEL_SIZE] = {0};
	snprintf(label, sizeof(label), "%s %s", "id:", expression->identifier->name);

	FILE *out = data;
	print_dot_node(out, expression, label);
}

static void print_dot_literal_int(struct mcc_ast_literal *literal, void *data)
{
	assert(literal);
	assert(data);

	char label[LABEL_SIZE] = {0};
	snprintf(label, sizeof(label), "%ld", literal->i_value);

	FILE *out = data;
	print_dot_node(out, literal, label);
}

static void print_dot_literal_float(struct mcc_ast_literal *literal, void *data)
{
	assert(literal);
	assert(data);

	char label[LABEL_SIZE] = {0};
	snprintf(label, sizeof(label), "%f", literal->f_value);

	FILE *out = data;
	print_dot_node(out, literal, label);
}

static void print_dot_literal_bool(struct mcc_ast_literal *literal, void *data)
{
	assert(literal);
	assert(data);

	char label[LABEL_SIZE] = {0};
	char *bool_value[5];
	if (literal->b_value == 0)
		*bool_value = "false";
	else
		*bool_value = "true";
	snprintf(label, sizeof(label), "%s", *bool_value);

	FILE *out = data;
	print_dot_node(out, literal, label);
}

static void print_dot_literal_string(struct mcc_ast_literal *literal, void *data)
{
	assert(literal);
	assert(data);

	char label[LABEL_SIZE] = {0};
	snprintf(label, sizeof(label), "%s", literal->str_value);

	FILE *out = data;
	print_dot_node(out, literal, label);
}

static void print_dot_declaration(struct mcc_ast_declare_assign *declaration, void *data)
{
	assert(declaration);
	FILE *out = data;

	char label[LABEL_SIZE] = {0};
	snprintf(label, sizeof(label), "%s %d", "dec:", declaration->type);

	print_dot_node(out, declaration, label);
	print_dot_edge(out, declaration, declaration->declare_id, "id");
}

static void print_dot_assignment(struct mcc_ast_declare_assign *assignment, void *data)
{
	assert(assignment);
	FILE *out = data;

	print_dot_node(out, assignment, "assign");

	print_dot_edge(out, assignment, assignment->assign_lhs, "lhs");
	print_dot_edge(out, assignment, assignment->assign_rhs, "rhs");
}

static void print_dot_statement_expression(struct mcc_ast_statement *statement, void *data)
{
	assert(statement);
	FILE *out = data;

	print_dot_node(out, statement, "stmt: expr");
	print_dot_edge(out, statement, statement->expression, "expr");
}

static void print_dot_statement_declaration(struct mcc_ast_statement *statement, void *data)
{
	assert(statement);
	FILE *out = data;

	print_dot_node(out, statement, "stmt: dec");
	print_dot_edge(out, statement, statement->declare_assign, "dec");
}

static void print_dot_statement_assignment(struct mcc_ast_statement *statement, void *data)
{
	assert(statement);
	FILE *out = data;

	print_dot_node(out, statement, "stmt: assign");
	print_dot_edge(out, statement, statement->declare_assign, "assign");
}

static void print_dot_statement_return(struct mcc_ast_statement *statement, void *data)
{
	assert(statement);
	FILE *out = data;

	print_dot_node(out, statement, "return");
	print_dot_edge(out, statement, statement->expression, "expr.");
}

static void print_dot_statement_if(struct mcc_ast_statement *statement, void *data)
{
	assert(statement);
	FILE *out = data;

	print_dot_node(out, statement, "stmt: if");

	print_dot_edge(out, statement, statement->if_cond, "cond.");

	print_dot_edge(out, statement, statement->if_stat, "if");
	print_dot_edge(out, statement, statement->else_stat, "else");
}

static void print_dot_statement_while(struct mcc_ast_statement *statement, void *data)
{
	assert(statement);
	FILE *out = data;

	print_dot_node(out, statement, "stmt: while");

	print_dot_edge(out, statement, statement->while_cond, "cond.");
	print_dot_edge(out, statement, statement->while_stat, "body");
}

static void print_dot_statement_compound(struct mcc_ast_statement *statement, void *data)
{
	assert(statement);
	FILE *out = data;

	print_dot_node(out, statement, "stmt: block");
	struct mcc_ast_statement_list *comp_stmt = statement->compound;
	int i = 0;
	while (comp_stmt != NULL) {
		char label[LABEL_SIZE] = {0};
		snprintf(label, sizeof(label), "%s %d", "stmt:", i++);
		print_dot_edge(out, statement, comp_stmt->statement, label);
		comp_stmt = comp_stmt->next_statement;
	}
}

// Setup an AST Visitor for printing.
static struct mcc_ast_visitor print_dot_visitor(FILE *out)
{
	assert(out);

	return (struct mcc_ast_visitor){
	    .traversal = MCC_AST_VISIT_DEPTH_FIRST,
	    .order = MCC_AST_VISIT_PRE_ORDER,

	    .userdata = out,

	    .expression_literal = print_dot_expression_literal,
	    .expression_binary_op = print_dot_expression_binary_op,
	    .expression_unary_op = print_dot_expression_unary_op,
	    .expression_parenth = print_dot_expression_parenth,
	    .expression_identifier = print_dot_expression_identifier,

	    .literal_int = print_dot_literal_int,
	    .literal_float = print_dot_literal_float,
	    .literal_bool = print_dot_literal_bool,
	    .literal_string = print_dot_literal_string,

	    .declaration = print_dot_declaration,
	    .assignment = print_dot_assignment,

	    .statement_expression = print_dot_statement_expression,
	    .statement_return = print_dot_statement_return,
	    .statement_if = print_dot_statement_if,
	    .statement_while = print_dot_statement_while,
	    .statement_compound = print_dot_statement_compound,
	};
}

void mcc_ast_print_dot_program(FILE *out, struct mcc_ast_program *program)
{
	assert(out);
	assert(program);

	print_dot_begin(out);

	struct mcc_ast_visitor visitor = print_dot_visitor(out);
	mcc_ast_visit(program, &visitor);

	print_dot_end(out);
}
