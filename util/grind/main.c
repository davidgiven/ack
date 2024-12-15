/* $Id$ */

#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <stdarg.h>
#include <string.h>
#include <unistd.h>

#include "tokenname.h"
#include "position.h"
#include "file.h"
#include "symbol.h"
#include "scope.h"
#include "Lpars.h"
#include "type.h"
#include "langdep.h"
#include "misc.h"
#include "run.h"

static char* usage = "Usage: %s [<ack.out>] [<a.out>]";
char* progname;
char* AckObj;
char* AObj;
char* dirs[] = { "", 0 };
FILE* db_out;
FILE* db_in;
int debug;
extern struct tokenname tkidf[];
extern int eof_seen;
extern int interrupted;
p_file h_file;

static struct tokenname shorts[]
    = { { LIST, "l" },  { CONT, "c" },    { STEP, "s" },  { NEXT, "n" },  { DELETE, "d" },
	    { PRINT, "p" }, { RESTORE, "r" }, { TRACE, "t" }, { WHERE, "w" }, { 0, 0 } };

int main(int argc, char* argv[])
{
	char* p;

	db_out = stdout;
	db_in = stdin;
	progname = argv[0];
	init_del();
	while (p = strchr(progname, '/'))
	{
		progname = p + 1;
	}
	while (argv[1] && argv[1][0] == '-')
	{
		switch (argv[1][1])
		{
			case 'v':
				debug++;
				break;
			case 'i':
				int_size = atoi(&argv[1][2]);
				break;
			case 's':
				short_size = atoi(&argv[1][2]);
				break;
			case 'l':
				long_size = atoi(&argv[1][2]);
				break;
			case 'f':
				float_size = atoi(&argv[1][2]);
				break;
			case 'd':
				double_size = atoi(&argv[1][2]);
				break;
			case 'p':
				pointer_size = atoi(&argv[1][2]);
				break;
			default:
				fatal(usage, progname);
		}
		argv++;
		argc--;
	}
	if (argc > 3)
	{
		fatal(usage, progname);
	}
	AckObj = argv[1] ? argv[1] : "a.out";
	if (argc == 3)
		AObj = argv[2];
	init_types();
	init_scope();
	init_languages();
	if (DbRead(AckObj) && AObj == 0)
		AObj = AckObj;
	else if (AObj == 0)
		AObj = "a.out";
	reserve(tkidf);
	reserve(shorts);
	init_run();
	if (!currlang)
	{
		fatal("could not determine source language. Recompile with -g?");
	}
	prompt();
	Commands();
	signal_child(SIGKILL);
	if (eof_seen)
		putc('\n', db_out);
	exit(0);
}

void prompt(void)
{
	if (isatty(fileno(db_in)))
	{
		fprintf(db_out, "-> ");
		fflush(db_out);
	}
}

extern int errorgiven;

void fatal(char* fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	{
		fprintf(db_out, "%s: ", progname);
		vfprintf(db_out, fmt, ap);
		fprintf(db_out, "\n");
	}
	va_end(ap);
	exit(1);
}

void error(char* fmt, ...)
{
	va_list ap;

	if (!interrupted)
	{
		va_start(ap, fmt);
		{
			fprintf(db_out, "%s: ", progname);
			vfprintf(db_out, fmt, ap);
			fprintf(db_out, "\n");
		}
		va_end(ap);
	}
	errorgiven = 1;
}

void warning(char* fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	{
		fprintf(db_out, "%s: ", progname);
		vfprintf(db_out, fmt, ap);
		fprintf(db_out, "\n");
	}
	va_end(ap);
}

void rd_fatal(void)
{
	fatal("read error in %s", AckObj);
}
