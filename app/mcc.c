#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "mcc/asm.h"
#include "mcc/ast.h"
#include "mcc/ast_print.h"
#include "mcc/ir.h"
#include "mcc/parser.h"
#include "mcc/symbol_table.h"
#include "mcc/type_checking.h"

enum log_level { LOG_DEFAULT, LOG_INFO, LOG_DEBUG };

int log_level_to_int(enum log_level level)
{
	switch (level) {
	case LOG_DEFAULT:
		return 0;
	case LOG_INFO:
		return 1;
	case LOG_DEBUG:
		return 2;
	}
	return 0;
}

void print_help(const char *prg_name)
{
	printf("usage: %s [OPTIONS] file...\n", prg_name);
	printf("\nThe mc compiler. \n");
	printf("\nUse '-' as input file to read from stdin.\n");
	printf("OPTIONS:\n");
	printf("\t-h,--help \t\tdisplays this help message\n");
	printf("\t-o,--output <file> \twrite the output to <file> (defaults to stdout)\n");
	printf("\t-f, --function <name> \tlimit scope to the given function\n");
}

/* compile assembly with
        gcc -c file.s -o file.o
        gcc -o file file.o

        other way:
                gcc -S -m32 hello.c
                gcc -o hello_asm -m32 hello.s

        for debugging with gdb:
                as --gstabs+ test.s -o test.o --32
                ld -m elf_i386 test.o -o test
                gdb test
                b main
                run
                si and to repeat ENTER
                q for quitting gdb

*/
void mcc_invoke_backend(char *file_name, char *output_name)
{
	assert(file_name);

	char file_name_built_in[] = "../resources/mc_builtins.c";

	char *argument[7];
	argument[0] = "gcc";
	argument[1] = "-m32";
	argument[2] = file_name_built_in;
	argument[3] = file_name;
	argument[4] = "-o";
	argument[5] = output_name;
	argument[6] = NULL;

	execvp("gcc", argument);
}

int main(int argc, char *argv[])
{
	enum log_level LOG_LEVEL = LOG_DEFAULT;
	if (getenv("MCC_LOG_LEVEL")) {
		char *log_level_env = getenv("MCC_LOG_LEVEL");
		if (strcmp(log_level_env, "1") == 0) {
			LOG_LEVEL = LOG_INFO;
		}
		if (strcmp(log_level_env, "2") == 0) {
			LOG_LEVEL = LOG_DEBUG;
		}
	}

	static struct option long_options[] = {{"help", no_argument, NULL, 'h'},
	                                       {"output", optional_argument, NULL, 'o'},
	                                       {"function", optional_argument, NULL, 'f'}};

	char func_name_scope[64] = {0};
	int isScoped = 0;
	char outfile[64] = {0};
	char *outfile_env = getenv("MCC_LOG_FILE");
	if (outfile_env == NULL || strcmp(outfile_env, "") == 0) {
		snprintf(outfile, sizeof(outfile), "%s", "-");
	} else {
		snprintf(outfile, sizeof(outfile), "%s", outfile_env);
	}
	int c;
	while ((c = getopt_long(argc, argv, "ho:f:", long_options, NULL)) != -1)
		switch (c) {
		case 'h':
			print_help(argv[0]);
			return EXIT_SUCCESS;
			break;
		case 'o':
			snprintf(outfile, sizeof(outfile), "%s", optarg);
			break;
		case 'f':
			isScoped = 1;
			snprintf(func_name_scope, sizeof(func_name_scope), "%s", optarg);
			break;
		default:
			return EXIT_FAILURE;
		}

	// get output file / stdout
	FILE *out;
	if (strcmp("-", outfile) == 0) {
		out = stdout;
	} else {
		out = fopen(outfile, "w");
		if (!out) {
			perror("fopen");
			return EXIT_FAILURE;
		}
	}

	for (int i = optind; i < argc; i++) { // loop through given files

		// get input file / stdin
		FILE *in;
		if (strcmp("-", argv[i]) == 0) {
			in = stdin;
		} else {
			in = fopen(argv[i], "r");
			if (!in) {
				perror("fopen");
				return EXIT_FAILURE;
			}
		}

		struct mcc_ast_program *pro = NULL;
		struct mcc_symbol_table *st = NULL;
		struct mcc_ir_table_head *ir = NULL;

		// parsing phase
		{
			struct mcc_parser_result result = mcc_parse_file(in, argv[i], out, log_level_to_int(LOG_LEVEL));

			if (result.status != MCC_PARSER_STATUS_OK) {
				fprintf(stdout, "...parsing failed...\n");
				fclose(in);
				return EXIT_FAILURE;
			}
			pro = result.program;
		}

		if (isScoped) {
			struct mcc_ast_func_list *list = pro->function_list;
			struct mcc_ast_func_list *scope_func_list;
			while (list != NULL) {
				char *id = list->function->func_identifier->identifier->name;
				if (strcmp(func_name_scope, id) == 0) {
					scope_func_list = list;
				}
				list = list->next_function;
			}
			pro->function_list = scope_func_list;
			pro->function_list->next_function = NULL;
		}

		// build symbol table
		st = mcc_create_symbol_table(pro, out, log_level_to_int(LOG_LEVEL));
		if (st == NULL) {
			mcc_ast_delete_program(pro);
			fclose(in);
			return EXIT_FAILURE;
		}

		// type checking
		int error = mcc_check_types(pro, st, out, log_level_to_int(LOG_LEVEL));

		if (error) {
			mcc_delete_symbol_table(st);
			mcc_ast_delete_program(pro);
			fclose(in);
			return EXIT_FAILURE;
		}

		// generate IR code
		ir = mcc_create_ir(pro, out, log_level_to_int(LOG_LEVEL));

		// generate ASM code
		// mcc_create_asm(ir, out, 1);

		mcc_invoke_backend("asm_tmp.s", "output.o");

		// cleanup
		mcc_delete_ir(ir);
		mcc_delete_symbol_table(st);
		mcc_ast_delete_program(pro);

		if (fclose(in) != 0) {
			perror("fclose input");
			return EXIT_FAILURE;
		}
	}
	if (fclose(out) != 0) {
		perror("fclose output");
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}