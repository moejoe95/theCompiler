#ifndef MCC_ERROR_HANDLER_H
#define MCC_ERROR_HANDLER_H

#include "parser.tab.h"

void print_lexer_error(char* filename, struct MCC_PARSER_LTYPE *yylloc, const char *msg);
void print_scanner_error(char error_char);

#endif // MCC_ERROR_HANDLER_H