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
	printf("\t-o,--output <file> \twrite the output to <file> (defaults to a.out)\n");
	printf("\t-v, --version <name> \tdisplay compiler version\n");
	printf("\t-q, --quiet <name> \tsuppress error output\n");
}

void mcc_invoke_backend(char *gcc, char *file_name, char *output_name)
{
	assert(file_name);

	char file_name_built_in[] = "../resources/mc_builtins.c";

	char command[128];
	strcpy(command, gcc);
	strcat(command, " -m32 ");
	strcat(command, file_name_built_in);
	strcat(command, " ");
	strcat(command, file_name);
	strcat(command, " -o ");
	strcat(command, output_name);
	system(command);
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

	char *gcc_path = "gcc";
	if (getenv("MCC_BACKEND")) {
		gcc_path = getenv("MCC_BACKEND");
	}

	static struct option long_options[] = {{"help", no_argument, NULL, 'h'},
	                                       {"output", optional_argument, NULL, 'o'},
	                                       {"version", optional_argument, NULL, 'v'},
	                                       {"quiet", optional_argument, NULL, 'q'}};

	char outfile[64] = {0};
	snprintf(outfile, sizeof(outfile), "%s", "a.out");

	char logfile[64] = {0};
	char *logfile_env = getenv("MCC_LOG_FILE");
	if (logfile_env == NULL || strcmp(logfile_env, "") == 0) {
		snprintf(logfile, sizeof(logfile), "%s", "-");
	} else {
		snprintf(logfile, sizeof(logfile), "%s", logfile_env);
	}
	int c;
	while ((c = getopt_long(argc, argv, "ho:vq:", long_options, NULL)) != -1)
		switch (c) {
		case 'h':
			print_help(argv[0]);
			return EXIT_SUCCESS;
			break;
		case 'o':
			snprintf(outfile, sizeof(outfile), "%s", optarg);
			break;
		case 'v':
			printf("compiler version 0.1\n");
			break;
		case 'q':
			// TODO suppress error output
			break;
		default:
			return EXIT_FAILURE;
		}

	// get log file / stdout
	FILE *out;
	if (strcmp("-", logfile) == 0) {
		out = stdout;
	} else {
		out = fopen(logfile, "w");
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
				mcc_ast_delete_program(result.program);
				fclose(in);
				return EXIT_FAILURE;
			}
			pro = result.program;
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
		mcc_create_asm(ir, out, log_level_to_int(LOG_LEVEL) + 1);

		// generate binary from ASM
		mcc_invoke_backend(gcc_path, "asm_tmp.s", outfile);

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
