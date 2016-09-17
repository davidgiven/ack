#include "mcg.h"
#include "em_comp.h"

void fatal(const char* msg, ...)
{
	va_list ap;
	va_start(ap, msg);

	vfprintf(stderr, msg, ap);
	fprintf(stderr, "\n");

	va_end(ap);
	abort();
}

int main(int argc, char* argv[])
{
	if (!EM_open(argv[1]))
		fatal("Couldn't open input file: %s", EM_error);
	
    parse_em();

	EM_close();
	return 0;
}

/* vim: set sw=4 ts=4 expandtab : */
