#ifndef MCC_ERROR_HANDLER_H
#define MCC_ERROR_HANDLER_H

enum semantic_error_type { //todo other names?
	MCC_SC_ERROR_NO_MAIN
};

struct mcc_semantic_error {
	enum semantic_error_type error_type;
	struct mcc_ast_source_location *sloc;
};
struct mcc_semantic_error *get_mcc_semantic_error_struct(enum semantic_error_type error_type, struct mcc_ast_source_location *sloc);

void print_lexer_error(char* filename, int last_line, int last_column, const char *msg);
void print_scanner_error(char* filename, int last_line, int last_column, char error_char);
void print_semantic_error(struct mcc_semantic_error *semantic_error);

#endif // MCC_ERROR_HANDLER_H