#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include "diagnostics.h"

const char* program_name = NULL;

void warning(const char* msg, ...)
{
	va_list ap;
	va_start(ap, msg);

	if (program_name)
		fprintf(stderr, "%s: ", program_name);
	fprintf(stderr, "warning: ");
	vfprintf(stderr, msg, ap);
	fprintf(stderr, "\n");

	va_end(ap);
}

void fatal(const char* msg, ...)
{
	va_list ap;
	va_start(ap, msg);

	if (program_name)
		fprintf(stderr, "%s: ", program_name);
	fprintf(stderr, "fatal: ");
	vfprintf(stderr, msg, ap);
	fprintf(stderr, "\n");

	va_end(ap);
	abort();
}

/* vim: set sw=4 ts=4 expandtab : */

