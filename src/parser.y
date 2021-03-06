%define api.prefix {mcc_parser_}

%define api.pure full
%lex-param   {void *scanner}
%parse-param {void *scanner} {struct mcc_ast_program** result}

%define parse.trace
%define parse.error verbose

%code requires {
#include "mcc/parser.h"
#include "mcc/error_handler.h"

char *filename; /* current filename here for the lexer */
FILE *out;

typedef struct YYLTYPE {
	int first_line;
	int first_column;
	int last_line;
	int last_column;
	char *filename;
	FILE *out;
} YYLTYPE;

#define MCC_PARSER_LTYPE_IS_DECLARED 1
#define YYLLOC_DEFAULT(Current, Rhs, N)                                 \
		do                                                                  \
		if (N)                                                            \
			{                                                               \
			(Current).first_line   = YYRHSLOC (Rhs, 1).first_line;        \
			(Current).first_column = YYRHSLOC (Rhs, 1).first_column;            \
			(Current).last_line    = YYRHSLOC (Rhs, N).last_line;           \
			(Current).last_column  = YYRHSLOC (Rhs, N).last_column;               \
			(Current).filename = YYRHSLOC (Rhs, 1).filename;              \
			(Current).out = YYRHSLOC (Rhs, 1).out;              \
			}                                                               \
		else                                                              \
			{                                                               \
			(Current).first_line   = (Current).last_line   =               \
				YYRHSLOC (Rhs, 0).last_line;                                 \
			(Current).first_column = (Current).last_column =                     \
				YYRHSLOC (Rhs, 0).last_column;                                  \
			}                                                               \
		while (0)
}

%{
#include <string.h>
#include <stdio.h>

int mcc_parser_lex();
void mcc_parser_error();
char *filename;
FILE *out;

#define loc(ast_node, ast_sloc) \
	(ast_node)->node.sloc.start_col = (ast_sloc).first_column + 1; \
	(ast_node)->node.sloc.start_line = (ast_sloc).first_line + 1; \
	(ast_node)->node.sloc.end_col = (ast_sloc).last_column + 1; \
	(ast_node)->node.sloc.end_line = (ast_sloc).last_line + 1; \
	(ast_node)->node.sloc.filename = filename; \

%}

%define api.value.type union
%define api.token.prefix {TK_}

%locations

%token END 0 "EOF"

%token <char *> IDENTIFIER      "identifier"
%token <long>   INT_LITERAL   	"integer literal"
%token <double> FLOAT_LITERAL 	"float literal"
%token <int> 	BOOL_LITERAL 	"bool literal"
%token <char *> STRING_LITERAL 	"string literal"

%token LPARENTH "("
%token RPARENTH ")"
%token LSQUAREBRACKET "["
%token RSQUAREBRACKET "]"
%token COMMA ","
%token SEMICOLON  ";"

%token PLUS  "+"
%token MINUS "-"
%token ASTER "*"
%token SLASH "/"

%token ST "<"
%token GT ">"

%token SE "<="
%token GE ">="

%token LAND "&&"
%token LOR "||"
%token NOT "!"

%token EQ "=="
%token NEQ "!="
%token ASSIGN "="

%token BOOL                     "bool"
%token INT                      "int"
%token FLOAT                    "float"
%token STRING                   "string"
%token VOID                     "void"
%token LCURLYBRACKET            "{"
%token RCURLYBRACKET            "}"

%token IF                       "if"
%token ELSE                     "else"
%token WHILE                    "while"
%token RETURN                   "return"

%precedence THEN
%precedence ELSE

%left LOR
%left LAND
%left EQ NEQ
%left ST GT SE GE 
%left PLUS MINUS
%left ASTER SLASH
%precedence UMINUS NOT

%type <enum mcc_ast_type>					type
%type <struct mcc_ast_literal *>			literal
%type <struct mcc_ast_expression *>			expression id function_call
%type <struct mcc_ast_parameter *>			parameters
%type <struct mcc_ast_function_arguments *> argument_list
%type <struct mcc_ast_declare_assign *> 	declaration assignment
%type <struct mcc_ast_statement *>			statement if_stmt while_stmt compound_stmt return
%type <struct mcc_ast_func_definition *>	function_def
%type <struct mcc_ast_statement_list *>		statement_list
%type <struct mcc_ast_func_list *>			func_list
%type <struct mcc_ast_program *>			program

%destructor { mcc_ast_delete_func_list($$); }           func_list
%destructor { mcc_ast_delete_func_definition($$); }     function_def
%destructor { mcc_ast_delete_parameter($$); }           parameters
%destructor { mcc_ast_delete_statement($$); }           statement return if_stmt while_stmt compound_stmt
%destructor { mcc_ast_delete_statement_list($$); }      statement_list
%destructor { mcc_ast_delete_expression($$); }          expression id function_call
%destructor { mcc_ast_delete_declare_assign($$); }      declaration assignment
%destructor { mcc_ast_delete_function_arguments($$); }  argument_list
%destructor { free($$); }                               STRING_LITERAL IDENTIFIER


%start program

%%

program : expression { *result = mcc_ast_new_program($1, MCC_AST_PROGRAM_TYPE_EXPRESSION); loc($$, @1);}
 		| assignment { *result = mcc_ast_new_program($1, MCC_AST_PROGRAM_TYPE_DECLARATION); loc($$, @1);}
        | declaration { *result = mcc_ast_new_program($1, MCC_AST_PROGRAM_TYPE_DECLARATION); loc($$, @1);}
        | statement { *result = mcc_ast_new_program($1, MCC_AST_PROGRAM_TYPE_STATEMENT); loc($$, @1);}
        | END { *result = mcc_ast_new_empty_program(filename);}
		| func_list { *result = mcc_ast_new_program($1, MCC_AST_PROGRAM_TYPE_FUNCTION_LIST); loc($$, @1);}
		;

func_list  : function_def { $$ = mcc_ast_new_function_list($1, NULL); loc($$, @1);}
           | function_def func_list { $$ = mcc_ast_new_function_list($1, $2); loc($$, @1);}
           ;

type : BOOL    { $$ = MCC_AST_TYPE_BOOL; } 
     | INT     { $$ = MCC_AST_TYPE_INT; }
     | FLOAT   { $$ = MCC_AST_TYPE_FLOAT; }
     | STRING  { $$ = MCC_AST_TYPE_STRING; }
     ;

literal : BOOL_LITERAL { $$ = mcc_ast_new_literal_bool($1); loc($$, @1); }
		| INT_LITERAL   { $$ = mcc_ast_new_literal_int($1);   loc($$, @1); }
		| FLOAT_LITERAL { $$ = mcc_ast_new_literal_float($1); loc($$, @1); }
		| STRING_LITERAL { $$ = mcc_ast_new_literal_string($1); loc($$, @1); }
		;

expression : id
		   | id LSQUAREBRACKET expression RSQUAREBRACKET { $$ = mcc_ast_new_expression_array_access($1, $3); loc($$, @1);}
		   | literal              		  	{ $$ = mcc_ast_new_expression_literal($1);                              loc($$, @1); }
           | expression PLUS  expression  	{ $$ = mcc_ast_new_expression_binary_op(MCC_AST_BINARY_OP_ADD, $1, $3); loc($$, @1); }
           | expression MINUS expression  	{ $$ = mcc_ast_new_expression_binary_op(MCC_AST_BINARY_OP_SUB, $1, $3); loc($$, @1); }
           | expression ASTER expression  	{ $$ = mcc_ast_new_expression_binary_op(MCC_AST_BINARY_OP_MUL, $1, $3); loc($$, @1); }
           | expression SLASH expression  	{ $$ = mcc_ast_new_expression_binary_op(MCC_AST_BINARY_OP_DIV, $1, $3); loc($$, @1); }
           | LPARENTH expression RPARENTH 	{ $$ = mcc_ast_new_expression_parenth($2);                              loc($$, @1); }
		   | expression ST expression 		{ $$ = mcc_ast_new_expression_binary_op(MCC_AST_BINARY_OP_ST, $1, $3); loc($$, @1); }
		   | expression GT expression 		{ $$ = mcc_ast_new_expression_binary_op(MCC_AST_BINARY_OP_GT, $1, $3); loc($$, @1); }
		   | MINUS expression %prec UMINUS  { $$ = mcc_ast_new_expression_unary_op(MCC_AST_UNARY_OP_MINUS, $2); loc($$, @1); }
		   | NOT expression					{ $$ = mcc_ast_new_expression_unary_op(MCC_AST_UNARY_OP_NOT, $2); loc($$, @1); }
		   | expression SE expression		{ $$ = mcc_ast_new_expression_binary_op(MCC_AST_BINARY_OP_SE, $1, $3); loc($$, @1); }	
		   | expression GE expression 		{ $$ = mcc_ast_new_expression_binary_op(MCC_AST_BINARY_OP_GE, $1, $3); loc($$, @1); }		 
		   | expression EQ expression 		{ $$ = mcc_ast_new_expression_binary_op(MCC_AST_BINARY_OP_EQ, $1, $3); loc($$, @1); }	
		   | expression NEQ expression  	{ $$ = mcc_ast_new_expression_binary_op(MCC_AST_BINARY_OP_NEQ, $1, $3); loc($$, @1); }	
		   | expression LAND expression 	{ $$ = mcc_ast_new_expression_binary_op(MCC_AST_BINARY_OP_LAND, $1, $3); loc($$, @1); }
		   | expression LOR expression		{ $$ = mcc_ast_new_expression_binary_op(MCC_AST_BINARY_OP_LOR, $1, $3); loc($$, @1); }
		   | function_call
		   ;

id : IDENTIFIER  { $$ = mcc_ast_new_expression_identifier($1); loc($$, @1);}
   ;

parameters : declaration { $$ = mcc_ast_new_parameter($1, NULL); loc($$, @1);}
           | declaration COMMA parameters { $$ = mcc_ast_new_parameter($1, $3); loc($$, @1);}
           ;

declaration : type id { $$ = mcc_ast_new_declaration($1, $2, 0, 0); loc($$, @1);}
			| type LSQUAREBRACKET INT_LITERAL RSQUAREBRACKET id  { $$ = mcc_ast_new_declaration($1, $5, $3, 1); loc($$, @1);}
            ;

assignment : id ASSIGN expression { $$ = mcc_ast_new_assignment($1, $3, NULL); loc($$, @1);}
           | id LSQUAREBRACKET expression RSQUAREBRACKET ASSIGN expression { $$ = mcc_ast_new_assignment($1, $6, $3); loc($$, @1);}
           ;

statement : declaration SEMICOLON  { $$ = mcc_ast_new_statement_declaration($1); loc($$, @1);}
 		  | assignment SEMICOLON   { $$ = mcc_ast_new_statement_assignment($1); loc($$, @1);}
          | expression SEMICOLON   { $$ = mcc_ast_new_statement_expression($1); loc($$, @1);}
		  | if_stmt 
		  | while_stmt 
		  | return SEMICOLON
		  | compound_stmt
          ;

return : RETURN { $$ = mcc_ast_new_statement_return_expression(NULL); loc($$, @1);} 
	   | RETURN expression { $$ = mcc_ast_new_statement_return_expression($2); loc($$, @1);}
       ;
		   
if_stmt : IF LPARENTH expression RPARENTH statement ELSE statement  { $$ = mcc_ast_new_if_stmt($3, $5, $7); loc($$, @1);}
		| IF LPARENTH expression RPARENTH %prec THEN statement { $$ = mcc_ast_new_if_stmt($3, $5, NULL); loc($$, @1);}
        ;

while_stmt : WHILE LPARENTH expression RPARENTH statement { $$ = mcc_ast_new_while_stmt($3, $5); loc($$, @1);}
        ;

compound_stmt : LCURLYBRACKET RCURLYBRACKET {  $$ = mcc_ast_new_statement_compound(NULL); loc($$, @1);}
              | LCURLYBRACKET statement_list RCURLYBRACKET { $$ = mcc_ast_new_statement_compound($2); loc($$, @1);}
              ;

function_def : VOID id LPARENTH RPARENTH compound_stmt { $$ = mcc_ast_new_function(MCC_AST_TYPE_VOID, $2, $5, NULL); loc($$, @1);}
             | type id LPARENTH RPARENTH compound_stmt { $$ = mcc_ast_new_function($1, $2, $5, NULL); loc($$, @1);}
             | VOID id LPARENTH parameters RPARENTH compound_stmt { $$ = mcc_ast_new_function(MCC_AST_TYPE_VOID, $2, $6, $4); loc($$, @1);}
             | type id LPARENTH parameters RPARENTH compound_stmt { $$ = mcc_ast_new_function($1, $2, $6, $4); loc($$, @1);}
             ;

function_call : id LPARENTH RPARENTH { $$ = mcc_ast_new_expression_function_call($1, NULL); loc($$, @1);}
              | id LPARENTH argument_list RPARENTH { $$ = mcc_ast_new_expression_function_call($1, $3); loc($$, @1);}
              ;

argument_list : expression { $$ = mcc_ast_new_expression_argument($1, NULL); loc($$, @1);}
              | expression COMMA argument_list { $$ = mcc_ast_new_expression_argument($1, $3); loc($$, @1);}
              ;

statement_list : statement { $$ = mcc_ast_new_statement_compound_stmt($1, NULL); loc($$, @1);}
               | statement statement_list { $$ = mcc_ast_new_statement_compound_stmt($1, $2); loc($$, @1);}
               ;

%%

#include <assert.h>

#include "scanner.h"
#include "utils/unused.h"

void mcc_parser_error(struct MCC_PARSER_LTYPE *yylloc, yyscan_t *scanner, struct mcc_ast_program** result, const char *msg)
{
	print_lexer_error(yylloc->filename, yylloc->last_line + 1, yylloc->last_column + 1, msg, yylloc->out);
	
	UNUSED(result);
	UNUSED(scanner);
}

struct mcc_parser_result mcc_parse_string(const char *input, int log_level)
{
	assert(input);

	FILE *in = fmemopen((void *)input, strlen(input), "r");
	if (!in) {
		return (struct mcc_parser_result){
		    .status = MCC_PARSER_STATUS_UNABLE_TO_OPEN_STREAM,
		};
	}

	struct mcc_parser_result result = mcc_parse_file(in, "input", stdout, log_level);

	fclose(in);

	return result;
}

struct mcc_parser_result mcc_parse_file(FILE *input, char *input_filename, FILE *outfile, int log_level)
{
	assert(input);

	filename = input_filename;
	out = outfile;

	yyscan_t scanner;
	mcc_parser_lex_init(&scanner);
	mcc_parser_set_in(input, scanner);

	struct mcc_parser_result result = {
	    .status = MCC_PARSER_STATUS_OK,
	};

	if (yyparse(scanner, &result.program) != 0) {
		result.status = MCC_PARSER_STATUS_UNKNOWN_ERROR;
	}

	mcc_parser_lex_destroy(scanner);

	if (result.status == MCC_PARSER_STATUS_OK && log_level != 0) {
		mcc_ast_print_dot(out, result.program);
	}

	return result;
}
