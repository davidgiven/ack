#include "mcg.h"

void fatal(const char* msg, ...)
{
	va_list ap;
	va_start(ap, msg);

	vfprintf(stderr, msg, ap);
	fprintf(stderr, "\n");

	va_end(ap);
	abort();
}

const char* aprintf(const char* fmt, ...)
{
    int n;
    char* p;
    va_list ap;

    va_start(ap, fmt);
    n = vsnprintf(NULL, 0, fmt, ap) + 1;
    va_end(ap);

    p = malloc(n);
    if (!p)
        return NULL;

    va_start(ap, fmt);
    vsnprintf(p, n, fmt, ap);
    va_end(ap);

    return p;
}

int main(int argc, char* argv[])
{
    symbol_init();

	if (!EM_open(argv[1]))
		fatal("Couldn't open input file: %s", EM_error);
	
    parse_em();

	EM_close();
	return 0;
}

/* vim: set sw=4 ts=4 expandtab : */
