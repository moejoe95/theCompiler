%option prefix="mcc_parser_"

%option batch
%option bison-bridge
%option bison-locations
%option noinput
%option nounput
%option noyywrap
%option reentrant
%option yylineno

%{
#include "parser.tab.h"

#define YYSTYPE MCC_PARSER_STYPE
#define YYLTYPE MCC_PARSER_LTYPE

// TODO:
// - track lines
// - take line breaks into account
// perhaps have to change ignore in line 87 for \n
#define YY_USER_ACTION \
	yylloc->first_column = yylloc->last_column; \
	if(yytext == '\n'){ \
		yylloc->first_column = yylloc->last_column; \
		yylloc->last_column = 0; \
	} \
	else{ \
		yylloc->last_column += yyleng; \
	} \
	yylloc->last_line = yylineno; 
%}

alpha            [a-zA-Z_]
alpha_num        [a-zA-Z0-9_]
digit            [0-9]

int_literal		{digit}+
float_literal 	{digit}+\.{digit}+
bool_literal	"true"|"false"
string_literal	\"[^"]*\"
identifier		{alpha}{alpha_num}*

%%

"<expression>"                { return TK_EXPRESSION; }
"<declaration_assignment>"    { return TK_DECLARATION_ASSIGNMENT; }
"<statement>"                 { return TK_STATEMENT; }

"bool"            { return TK_BOOL; }
"float"           { return TK_FLOAT; }
"string"          { return TK_STRING; }
"int"             { return TK_INT; }
"void"            { return TK_VOID; }

"if"              { return TK_IF; }
"else"            { return TK_ELSE; }
"while"           { return TK_WHILE; }
"return"           { return TK_RETURN; }

{int_literal}     { yylval->TK_INT_LITERAL = atol(yytext); return TK_INT_LITERAL; }
{float_literal}   { yylval->TK_FLOAT_LITERAL = atof(yytext); return TK_FLOAT_LITERAL; }
{bool_literal}	  { yylval->TK_BOOL_LITERAL = strcmp(yytext, "true") == 0; return TK_BOOL_LITERAL; }
{string_literal}  { yylval->TK_STRING_LITERAL = strdup(yytext); return TK_STRING_LITERAL; }

{identifier}      { yylval->TK_IDENTIFIER = strdup(yytext); return TK_IDENTIFIER; }

"+"               { return TK_PLUS; }
"-"               { return TK_MINUS; }
"*"               { return TK_ASTER; }
"/"               { return TK_SLASH; }

"("               { return TK_LPARENTH; }
")"               { return TK_RPARENTH; }
"["               { return TK_LSQUAREBRACKET; }
"]"               { return TK_RSQUAREBRACKET; }
"{"               { return TK_LCURLYBRACKET; }
"}"               { return TK_RCURLYBRACKET; }

"<"               { return TK_ST; }
">"               { return TK_GT; }

"<="			  { return TK_SE; }
">="			  { return TK_GE; }

"&&"			  { return TK_LAND; }
"||"			  { return TK_LOR; }

"=="			  { return TK_EQ; }
"!="			  { return TK_NEQ; }

"="               { return TK_ASSIGN; }
";"               { return TK_SEMICOLON; }
","               { return TK_COMMA; }
"!"               { return TK_NOT; }

[ \t\r\n]+        { /* ignore */ }

<<EOF>>           { return TK_END; }

.                 { fprintf(stderr, "unknown character '%c'\n", yytext[0]); }
