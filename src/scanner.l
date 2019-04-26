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
#include "mcc/error_handler.h"

#define YYSTYPE MCC_PARSER_STYPE
//#define YYLTYPE MCC_PARSER_LTYPE

#define YY_USER_ACTION \
    yylloc->filename = filename; \
    yylloc->first_line = yylloc->last_line; \
    yylloc->first_column = yylloc->last_column; \
    for(int i = 0; yytext[i] != '\0'; i++) { \
        if(yytext[i] == '\n') { \
            yylloc->last_line++; \
            yylloc->last_column = 0; \
        } \
        else { \
            yylloc->last_column++; \
        } \
    }


%}

alpha            [a-zA-Z_]
alpha_num        [a-zA-Z0-9_]
digit            [0-9]

int_literal		{digit}+
float_literal 	{digit}+\.{digit}+
bool_literal	"true"|"false"
string_literal	\"[^"]*\"
identifier		{alpha}{alpha_num}*

%x comment

%%

"/*"                    { BEGIN(comment); }
<comment>[^*]*
<comment>"*"+[^*/\n]*
<comment>"*"+"/"        { BEGIN(INITIAL); }

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

.                 { print_scanner_error(yylloc->filename, yylloc->last_line, yylloc->last_column, yytext[0]); }
