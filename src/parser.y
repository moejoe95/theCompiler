%define api.prefix {mcc_parser_}

%define api.pure full
%lex-param   {void *scanner}
%parse-param {void *scanner} {struct mcc_ast_expression** result}

%define parse.trace
%define parse.error verbose

%code requires {
#include "mcc/parser.h"
}

%{
#include <string.h>

int mcc_parser_lex();
void mcc_parser_error();

#define loc(ast_node, ast_sloc) \
	(ast_node)->node.sloc.start_col = (ast_sloc).first_column;

%}

%define api.value.type union
%define api.token.prefix {TK_}

%locations

%token END 0 "EOF"

%token <char *> IDENTIFIER      "identifier"
%token <long>   INT_LITERAL   "integer literal"
%token <double> FLOAT_LITERAL "float literal"
%token <int> 	BOOL_LITERAL "bool literal"
%token <char *> STRING_LITERAL "string literal"

%token LPARENTH "("
%token RPARENTH ")"
%token LSQUAREBRACKET           "["
%token RSQUAREBRACKET           "]"
%token COLON                    ","

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

%token EXPRESSION               "<expression>"
%token DECLARATION_ASSIGNMENT   "<declaration_assignment>"
%token STATEMENT                "<statement>"

%left PLUS MINUS
%left ASTER SLASH

%type <struct mcc_ast_expression *> expression
%type <struct mcc_ast_literal *> literal
%type <enum mcc_ast_type> type
%type <struct mcc_ast_declare_assign *> declaration
%type <struct mcc_ast_declare_assign *> assignment
%type <struct mcc_ast_expression *> id
%type <struct mcc_ast_parameter *> parameters

%start toplevel

%%

toplevel : expression { *result = $1; }
		 | declaration { *result = $1; }
		 | assignment { *result = $1; }
         ;

expression : literal              		  { $$ = mcc_ast_new_expression_literal($1);                              loc($$, @1); }
           | expression PLUS  expression  { $$ = mcc_ast_new_expression_binary_op(MCC_AST_BINARY_OP_ADD, $1, $3); loc($$, @1); }
           | expression MINUS expression  { $$ = mcc_ast_new_expression_binary_op(MCC_AST_BINARY_OP_SUB, $1, $3); loc($$, @1); }
           | expression ASTER expression  { $$ = mcc_ast_new_expression_binary_op(MCC_AST_BINARY_OP_MUL, $1, $3); loc($$, @1); }
           | expression SLASH expression  { $$ = mcc_ast_new_expression_binary_op(MCC_AST_BINARY_OP_DIV, $1, $3); loc($$, @1); }
           | LPARENTH expression RPARENTH { $$ = mcc_ast_new_expression_parenth($2);                              loc($$, @1); }
		   | expression ST expression  { $$ = mcc_ast_new_expression_binary_op(MCC_AST_BINARY_OP_ST, $1, $3); loc($$, @1); }
		   | expression GT expression  { $$ = mcc_ast_new_expression_binary_op(MCC_AST_BINARY_OP_GT, $1, $3); loc($$, @1); }
		   | MINUS expression { $$ = mcc_ast_new_expression_unary_op(MCC_AST_UNARY_OP_MINUS, $2); loc($$, @1); }
		   | expression SE expression  { $$ = mcc_ast_new_expression_binary_op(MCC_AST_BINARY_OP_SE, $1, $3); loc($$, @1); }	
		   | expression GE expression  { $$ = mcc_ast_new_expression_binary_op(MCC_AST_BINARY_OP_GE, $1, $3); loc($$, @1); }		 
		   | expression EQ expression  { $$ = mcc_ast_new_expression_binary_op(MCC_AST_BINARY_OP_EQ, $1, $3); loc($$, @1); }	
		   | expression NEQ expression  { $$ = mcc_ast_new_expression_binary_op(MCC_AST_BINARY_OP_NEQ, $1, $3); loc($$, @1); }	
		   | expression LAND expression { $$ = mcc_ast_new_expression_binary_op(MCC_AST_BINARY_OP_LAND, $1, $3); loc($$, @1); }
		   | expression LOR expression { $$ = mcc_ast_new_expression_binary_op(MCC_AST_BINARY_OP_LOR, $1, $3); loc($$, @1); }
		   ;

literal : BOOL_LITERAL { $$ = mcc_ast_new_literal_bool($1); loc($$, @1); }
		| INT_LITERAL   { $$ = mcc_ast_new_literal_int($1);   loc($$, @1); }
		| FLOAT_LITERAL { $$ = mcc_ast_new_literal_float($1); loc($$, @1); }
		| STRING_LITERAL { $$ = mcc_ast_new_literal_string($1); loc($$, @1); }

declaration : type id                                            
              { $$ = mcc_ast_new_declaration($1, $2, 0, 0, &@$); }
            | type LSQUAREBRACKET INT_LITERAL RSQUAREBRACKET id  
              { $$ = mcc_ast_new_declaration($1, $5, $3, 1, &@$); }
            ;

assignment : id ASSIGN expression { $$ = mcc_ast_new_assignment($1, $3, NULL, &@$, &@2, NULL); }
           | id LSQUAREBRACKET expression RSQUAREBRACKET ASSIGN expression { $$ = mcc_ast_new_assignment($1, $6, $3, &@$, &@5, &@2); }
           ;


id : IDENTIFIER  { $$ = mcc_ast_new_expression_identifier($1, &@1); }
   ;

type : BOOL    { $$ = MCC_AST_TYPE_BOOL; }
     | INT     { $$ = MCC_AST_TYPE_INT; }
     | FLOAT   { $$ = MCC_AST_TYPE_FLOAT; }
     | STRING  { $$ = MCC_AST_TYPE_STRING; }
     ;

parameters : declaration { $$ = mcc_ast_new_parameter($1, NULL, &@$); }
           | declaration COLON parameters { $$ = mcc_ast_new_parameter($1, $3, &@$); }
           ;

%%

#include <assert.h>

#include "scanner.h"
#include "utils/unused.h"

void mcc_parser_error(struct MCC_PARSER_LTYPE *yylloc, yyscan_t *scanner, const char *msg)
{
	// TODO
	UNUSED(yylloc);
	UNUSED(scanner);
	UNUSED(msg);
}

struct mcc_parser_result mcc_parse_string(const char *input)
{
	assert(input);

	FILE *in = fmemopen((void *)input, strlen(input), "r");
	if (!in) {
		return (struct mcc_parser_result){
		    .status = MCC_PARSER_STATUS_UNABLE_TO_OPEN_STREAM,
		};
	}

	struct mcc_parser_result result = mcc_parse_file(in);

	fclose(in);

	return result;
}

struct mcc_parser_result mcc_parse_file(FILE *input)
{
	assert(input);

	yyscan_t scanner;
	mcc_parser_lex_init(&scanner);
	mcc_parser_set_in(input, scanner);

	struct mcc_parser_result result = {
	    .status = MCC_PARSER_STATUS_OK,
	};

	if (yyparse(scanner, &result.expression) != 0) {
		result.status = MCC_PARSER_STATUS_UNKNOWN_ERROR;
	}

	mcc_parser_lex_destroy(scanner);

	return result;
}
