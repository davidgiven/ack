#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdarg.h>
#include <getopt.h>
#include <ctype.h>
#include "globals.h"

bool flag_enter_debugger = false;
char* const* user_command_line = NULL;

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
	user_command_line = &argv[optind];
}

int main(int argc, char* const* argv)
{
	parse_options(argc, argv);

	#if 0
	emulator_init();
	bios_coldboot();
	
	for (;;)
	{
		emulator_run();
	}
	#endif

	return 0;
}


