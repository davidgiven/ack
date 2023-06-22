#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdarg.h>
#include <getopt.h>
#include <ctype.h>
#include <errno.h>
#include "globals.h"

bool flag_enter_debugger = false;
static char* command_path = NULL;
char* const* user_command_line = NULL;

uint8_t ram[RAMSIZE];

void fatal(const char* message, ...)
{
	va_list ap;
	va_start(ap, message);
	fprintf(stderr, "fatal: ");
	vfprintf(stderr, message, ap);
	fprintf(stderr, "\n");
	exit(1);
}

static void syntax(void)
{
	printf("mosemu [<flags>] command [args]:\n");
	printf("  -h             this help\n");
	printf("  -d             enter debugger on startup\n");
	exit(1);
}

static void parse_options(int argc, char* const* argv)
{
	for (;;)
	{
		switch (getopt(argc, argv, "hdp:"))
		{
			case -1:
				goto end_of_flags;

			case 'd':
				flag_enter_debugger = true;
				break;

			default:
				syntax();
		}
	}

end_of_flags:
	command_path = argv[optind++];
	if (!command_path)
		syntax();

	user_command_line = &argv[optind];
}

static void load_program(void)
{
	FILE* fp = fopen(command_path, "rb");
	if (!fp)
		fatal("could not load program: %s", strerror(errno));

	fread(&ram[0], sizeof(ram), 1, fp);
	fclose(fp);
}

int main(int argc, char* const* argv)
{
	parse_options(argc, argv);

	emulator_init();
	load_program();

	for (;;)
		emulator_run();

	return 0;
}
