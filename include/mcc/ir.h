#ifndef THECOMPILER_IR_H
#define THECOMPILER_IR_H

#include <stdio.h>
#include <string.h>

#include "mcc/ast.h"

enum ir_table_operation_type {
	MCC_IR_TABLE_UNARY_OP,
	MCC_IR_TABLE_BINARY_OP,
	MCC_IR_TABLE_ASSIGNMENT,
	MCC_IR_TABLE_JUMPFALSE,
	MCC_IR_TABLE_JUMP,
	MCC_IR_TABLE_LABEL,
	MCC_IR_TABLE_COPY,
	MCC_IR_TABLE_CALL,
	MCC_IR_TABLE_POP,
	MCC_IR_TABLE_PUSH,
	MCC_IR_TABLE_STORE,
	MCC_IR_TABLE_LOAD,
	MCC_IR_TABLE_BUILT_IN,
	MCC_IR_TABLE_NULL,
	MCC_IR_TABLE_RETURN
};

struct mcc_ir_table_head {
	struct mcc_ir_table *current;
	struct mcc_ir_table *root;
};

struct mcc_ir_table {
	char *func_name;
	struct mcc_ir_table *next_table;
	struct mcc_ir_line_head *line_head;
	int id;
};

struct mcc_ir_line {
	enum ir_table_operation_type op_type;
	union {
		enum mcc_ast_binary_op bin_op;
		enum mcc_ast_unary_op un_op;
	};
	char *built_in;

	char *arg1;
	char *arg2;
	int jump_target;
	int index;
	int memory_size;

	struct mcc_ast_node node;

	struct mcc_ir_line *next_line;
};

struct mcc_ir_line_head {
	struct mcc_ir_line *root;
	struct mcc_ir_line *current;

	struct mcc_ast_program *program;

	int index;
};

struct mcc_ast_visitor generate_ir_visitor(struct mcc_ir_line_head *head);
struct mcc_ir_table_head *mcc_create_ir(struct mcc_ast_program *program, FILE *out, int log_level);
void mcc_delete_ir(struct mcc_ir_table_head *table_head);

#endif