// Abstract Syntax Tree (AST)
//
// Here we define the AST data structure of the compiler. It mainly consists of
// *tagged unions* for node types and enums for operators.
//
// In addition to the node type specific members, each node features a common
// member `mmc_ast_node` which serves as a *base-class*. It holds data
// independent from the actual node type, like the source location.
//
// Also note that this makes excessive use of C11's *anonymous structs and
// unions* feature.

#ifndef MCC_AST_H
#define MCC_AST_H

// Forward Declarations
struct mcc_ast_literal;
struct mcc_ast_expression;
struct mcc_ast_identifier;
struct mcc_ast_parameter;
struct mcc_ast_function_arguments;
struct mcc_ast_declare_assign;
struct mcc_ast_new_declaration;
struct mcc_ast_new_assignment;
struct mcc_ast_statement;
struct mcc_ast_func_definition;
struct mcc_ast_statement_list;
struct mcc_ast_func_list;
struct mcc_ast_program;

// ------------------------------------------------------------------- AST Node

struct mcc_ast_source_location {
	int start_line;
	int start_col;
	int end_line;
	int end_col;
	char *filename;
};

struct mcc_ast_node {
	struct mcc_ast_source_location sloc;
};

void mcc_ast_delete_string(char *id);

// ------------------------------------------------------------------ Operators

enum mcc_ast_binary_op {
	MCC_AST_BINARY_OP_ADD,
	MCC_AST_BINARY_OP_SUB,
	MCC_AST_BINARY_OP_MUL,
	MCC_AST_BINARY_OP_DIV,
	MCC_AST_BINARY_OP_ST,
	MCC_AST_BINARY_OP_GT,
	MCC_AST_BINARY_OP_SE,
	MCC_AST_BINARY_OP_GE,
	MCC_AST_BINARY_OP_LAND,
	MCC_AST_BINARY_OP_LOR,
	MCC_AST_BINARY_OP_EQ,
	MCC_AST_BINARY_OP_NEQ,
	MCC_AST_BINARY_OP_NULL
};

enum mcc_ast_unary_op {
	MCC_AST_UNARY_OP_MINUS,
	MCC_AST_UNARY_OP_NOT,
};

char *get_bin_op_string(enum mcc_ast_binary_op op);
char *get_un_op_string(enum mcc_ast_unary_op op);

// -------------------------------------------------------------------- Types

enum mcc_ast_type {
	MCC_AST_TYPE_BOOL,
	MCC_AST_TYPE_INT,
	MCC_AST_TYPE_FLOAT,
	MCC_AST_TYPE_STRING,
	MCC_AST_TYPE_VOID,
	MCC_AST_TYPE_ARRAY,
	MCC_AST_TYPE_NULL
};

char *get_type_string(enum mcc_ast_type type);

// ------------------------------------------------------------------- Literals

enum mcc_ast_literal_type {
	MCC_AST_LITERAL_TYPE_INT,
	MCC_AST_LITERAL_TYPE_FLOAT,
	MCC_AST_LITERAL_TYPE_BOOL,
	MCC_AST_LITERAL_TYPE_STRING,
};

char *get_literal_type_string(enum mcc_ast_literal_type type);

struct mcc_ast_literal {
	struct mcc_ast_node node;

	enum mcc_ast_literal_type type;
	union {
		// MCC_AST_LITERAL_TYPE_INT
		long i_value;

		// MCC_AST_LITERAL_TYPE_FLOAT
		double f_value;

		// MCC_AST_LITERAL_TYPE_BOOL
		int b_value;

		// MCC_AST_LITERAL_TYPE_STRING
		char *str_value;
	};
};

struct mcc_ast_literal *mcc_ast_new_literal_int(long value);

struct mcc_ast_literal *mcc_ast_new_literal_float(double value);

struct mcc_ast_literal *mcc_ast_new_literal_bool(int value);

struct mcc_ast_literal *mcc_ast_new_literal_string(char *value);

void mcc_ast_delete_literal(struct mcc_ast_literal *literal);

// ---------------------------------------------------------------- Expressions

enum mcc_ast_expression_type {
	MCC_AST_EXPRESSION_TYPE_LITERAL,
	MCC_AST_EXPRESSION_TYPE_BINARY_OP,
	MCC_AST_EXPRESSION_TYPE_UNARY_OP,
	MCC_AST_EXPRESSION_TYPE_PARENTH,
	MCC_AST_EXPRESSION_TYPE_IDENTIFIER,
	MCC_AST_EXPRESSION_TYPE_ARRAY_ACCESS,
	MCC_AST_EXPRESSION_TYPE_FUNCTION_CALL
};

struct mcc_ast_expression {
	struct mcc_ast_node node;

	enum mcc_ast_expression_type type;
	enum mcc_ast_type expression_type;
	enum mcc_ast_type expression_array_type;
	union {
		// MCC_AST_EXPRESSION_TYPE_LITERAL
		struct mcc_ast_literal *literal;

		// MCC_AST_EXPRESSION_TYPE_BINARY_OP
		struct {
			enum mcc_ast_binary_op op;
			struct mcc_ast_expression *lhs;
			struct mcc_ast_expression *rhs;
		};

		// MCC_AST_EXPRESSION_TYPE_UNARY_OP
		struct {
			enum mcc_ast_unary_op u_op;
			struct mcc_ast_expression *u_rhs;
		};

		// MCC_AST_EXPRESSION_TYPE_PARENTH
		struct mcc_ast_expression *expression;

		// MCC_AST_EXPRESSION_TYPE_IDENTIFIER
		struct mcc_ast_identifier *identifier;

		// MCC_AST_EXPRESSION_TYPE_ARRAY_ACCESS
		struct {
			struct mcc_ast_expression *array_access_id;
			struct mcc_ast_expression *array_access_exp;
		};

		// MCC_AST_EXPRESSION_TYPE_FUNCTION_CALL
		struct {
			struct mcc_ast_expression *function_call_identifier;
			struct mcc_ast_function_arguments *function_call_arguments;
		};
	};
};

struct mcc_ast_function_arguments {
	struct mcc_ast_node node;
	struct mcc_ast_expression *expression;
	struct mcc_ast_function_arguments *next_argument;
};

struct mcc_ast_expression *mcc_ast_new_expression_literal(struct mcc_ast_literal *literal);

struct mcc_ast_expression *mcc_ast_new_expression_identifier(char *identifier);

struct mcc_ast_expression *mcc_ast_new_expression_binary_op(enum mcc_ast_binary_op op,
                                                            struct mcc_ast_expression *lhs,
                                                            struct mcc_ast_expression *rhs);

struct mcc_ast_expression *mcc_ast_new_expression_unary_op(enum mcc_ast_unary_op op, struct mcc_ast_expression *rhs);

struct mcc_ast_expression *mcc_ast_new_expression_parenth(struct mcc_ast_expression *expression);

struct mcc_ast_expression *mcc_ast_new_expression_function_call(struct mcc_ast_expression *identifier,
                                                                struct mcc_ast_function_arguments *arguments);

struct mcc_ast_function_arguments *mcc_ast_new_expression_argument(struct mcc_ast_expression *expression,
                                                                   struct mcc_ast_function_arguments *arguments);

struct mcc_ast_expression *mcc_ast_new_expression_array_access(struct mcc_ast_expression *identifier,
                                                               struct mcc_ast_expression *expression);

void mcc_ast_delete_function_arguments(struct mcc_ast_function_arguments *arguments);

void mcc_ast_delete_expression(struct mcc_ast_expression *expression);

// -------------------------------------------------------------------- Identifier

struct mcc_ast_identifier {
	struct mcc_ast_node node;
	char *name;
	enum mcc_ast_type type;
};

void mcc_ast_delete_identifier(struct mcc_ast_identifier *id);

// -------------------------------------------------------------------- Parameter

struct mcc_ast_parameter {
	struct mcc_ast_node node;
	struct mcc_ast_declare_assign *parameter;
	struct mcc_ast_parameter *next_parameter;
};

struct mcc_ast_parameter *mcc_ast_new_parameter(struct mcc_ast_declare_assign *declaration,
                                                struct mcc_ast_parameter *parameter);

void mcc_ast_delete_parameter(struct mcc_ast_parameter *parameter);

// ------------------------------------------------------------------- Declaration Assignment

enum mcc_ast_declare_assign_type {
	MCC_AST_TYPE_DECLARATION,
	MCC_AST_TYPE_ASSIGNMENT,
};

struct mcc_ast_declare_assign {
	struct mcc_ast_node node;

	enum mcc_ast_declare_assign_type type;

	union {
		// MCC_AST_TYPE_DECLARATION
		struct {
			enum mcc_ast_type declare_type;
			struct mcc_ast_expression *declare_id;
			long *declare_array_size;
		};

		// MCC_AST_TYPE_ASSIGNMENT
		struct {
			struct mcc_ast_expression *assign_lhs;
			struct mcc_ast_expression *assign_rhs;
		};
	};
};

void mcc_ast_delete_declare_assign(struct mcc_ast_declare_assign *declaration);

void mcc_ast_delete_declare_assign(struct mcc_ast_declare_assign *declaration);

struct mcc_ast_declare_assign *mcc_ast_new_assignment(struct mcc_ast_expression *identifier,
                                                      struct mcc_ast_expression *array_expression,
                                                      struct mcc_ast_expression *expression);

struct mcc_ast_declare_assign *
mcc_ast_new_declaration(enum mcc_ast_type type, struct mcc_ast_expression *identifier, long literal, int literal_flag);

// ------------------------------------------------------------------- Statements

enum mcc_ast_statement_type {
	MCC_AST_STATEMENT_EXPRESSION,
	MCC_AST_STATEMENT_ASSIGNMENT,
	MCC_AST_STATEMENT_DECLARATION,
	MCC_AST_STATEMENT_RETURN,
	MCC_AST_STATEMENT_IF,
	MCC_AST_STATEMENT_WHILE,
	MCC_AST_STATEMENT_COMPOUND,
};

struct mcc_ast_statement {
	struct mcc_ast_node node;
	enum mcc_ast_statement_type type;

	union {
		/* MCC_AST_STATEMENT_EXPRESSION MCC_AST_STATEMENT_RETURN*/
		struct mcc_ast_expression *expression;

		/* MCC_AST_STATEMENT_ASSIGNMENT MCC_AST_STATEMENT_DECLARATION */
		struct mcc_ast_declare_assign *declare_assign;

		/* MCC_AST_STATEMENT_IF */
		struct {
			struct mcc_ast_expression *if_cond;
			struct mcc_ast_statement *if_stat;
			struct mcc_ast_statement *else_stat;
		};

		/* MCC_AST_STATEMENT_WHILE */
		struct {
			struct mcc_ast_expression *while_cond;
			struct mcc_ast_statement *while_stat;
		};

		/* MCC_AST_STATEMENT_COMPOUND */
		struct mcc_ast_statement_list *compound;
	};
};

struct mcc_ast_statement_list {
	struct mcc_ast_node node;
	struct mcc_ast_statement *statement;
	struct mcc_ast_statement_list *next_statement;
};

struct mcc_ast_statement *mcc_ast_new_statement_expression(struct mcc_ast_expression *expression);

struct mcc_ast_statement *mcc_ast_new_statement_return_expression(struct mcc_ast_expression *expression);

struct mcc_ast_statement *mcc_ast_new_statement_assignment(struct mcc_ast_declare_assign *assignment);

struct mcc_ast_statement *mcc_ast_new_statement_declaration(struct mcc_ast_declare_assign *declaration);

struct mcc_ast_statement *
mcc_ast_new_if_stmt(struct mcc_ast_expression *, struct mcc_ast_statement *, struct mcc_ast_statement *);

struct mcc_ast_statement *mcc_ast_new_while_stmt(struct mcc_ast_expression *expression, struct mcc_ast_statement *);

struct mcc_ast_statement *mcc_ast_new_statement_return(struct mcc_ast_expression *return_expr);

struct mcc_ast_statement *mcc_ast_new_statement_compound(struct mcc_ast_statement_list *statement_list);

struct mcc_ast_statement_list *mcc_ast_new_statement_compound_stmt(struct mcc_ast_statement *statement,
                                                                   struct mcc_ast_statement_list *statement_list);

void mcc_ast_delete_statement(struct mcc_ast_statement *statement);

void mcc_ast_delete_statement_list(struct mcc_ast_statement_list *statement_list);

// -------------------------------------------------------------------- Functions

struct mcc_ast_func_definition {
	struct mcc_ast_node node;
	enum mcc_ast_type func_type;
	struct mcc_ast_expression *func_identifier;
	struct mcc_ast_parameter *parameter_list;
	struct mcc_ast_statement *func_compound;
};

struct mcc_ast_func_list {
	struct mcc_ast_node node;
	struct mcc_ast_func_definition *function;
	struct mcc_ast_func_list *next_function;
};

struct mcc_ast_func_definition *mcc_ast_new_function(enum mcc_ast_type type,
                                                     struct mcc_ast_expression *identifier,
                                                     struct mcc_ast_statement *compound,
                                                     struct mcc_ast_parameter *parameter);

struct mcc_ast_func_list *mcc_ast_new_function_list(struct mcc_ast_func_definition *function,
                                                    struct mcc_ast_func_list *function_list);

void mcc_ast_delete_func_definition(struct mcc_ast_func_definition *func_def);

void mcc_ast_delete_func_list(struct mcc_ast_func_list *func_list);

// ------------------------------------------------------------------- Program

enum mcc_ast_program_type {
	MCC_AST_PROGRAM_TYPE_EXPRESSION,
	MCC_AST_PROGRAM_TYPE_FUNCTION,
	MCC_AST_PROGRAM_TYPE_FUNCTION_LIST,
	MCC_AST_PROGRAM_TYPE_DECLARATION,
	MCC_AST_PROGRAM_TYPE_EMPTY,
	MCC_AST_PROGRAM_TYPE_STATEMENT
};

struct mcc_ast_program {
	struct mcc_ast_node node;
	enum mcc_ast_program_type type;
	union {
		struct mcc_ast_expression *expression;
		struct mcc_ast_declare_assign *declaration;
		struct mcc_ast_statement *statement;
		struct mcc_ast_func_definition *function;
		struct mcc_ast_func_list *function_list;
	};
};

struct mcc_ast_program *mcc_ast_new_program(void *program, enum mcc_ast_program_type type);
struct mcc_ast_program *mcc_ast_new_empty_program(char *filename);

void mcc_ast_delete_program(struct mcc_ast_program *program);

// -------------------------------------------------------------------- Utility

// clang-format off

#define mcc_ast_delete(x) _Generic((x), \
		struct mcc_ast_program *: mcc_ast_delete_program, \
		struct mcc_ast_expression *: mcc_ast_delete_expression, \
		struct mcc_ast_declare_assign *: mcc_ast_delete_declare_assign, \
		struct mcc_ast_literal *:    mcc_ast_delete_literal \
	)(x)

// clang-format on

#endif // MCC_AST_H
