#include "mcg.h"
#include <unistd.h>

static const char* tracechars = NULL;

bool tracing(char k)
{
    if (!tracechars)
        return false;

    return index(tracechars, k);
}

void tracef(char k, const char* fmt, ...)
{
    va_list ap;

    if (tracing(k))
    {
        va_start(ap, fmt);
        vprintf(fmt, ap);
        va_end(ap);
    }
}

static bool find_procedures_cb(struct symbol* symbol, void* user)
{
    if (symbol->proc)
        procedure_compile(symbol->proc);
    return false;
}

int main(int argc, char* const argv[])
{
    program_name = argv[0];

    opterr = 1;
    for (;;)
    {
        int c = getopt(argc, argv, "-d:");
        if (c == -1)
            break;

        switch (c)
        {
            case 'd':
                tracechars = optarg;
                break;

            case 1:
                fatal("unexpected argument '%s'", optarg);
        }
    }

    symbol_init();

	if (!EM_open(NULL))
		fatal("couldn't open stdin: %s", EM_error);
	
    /* Reads in the EM, outputs the data sections, parses any code and
     * generates IR trees. */

    parse_em();

    /* For every procedure, go ahead and do the compilation proper. We do this
     * now so that we know that all the data has been read correctly and our
     * symbol table is complete (we may need to refer to it). */

    symbol_walk(find_procedures_cb, NULL);

	EM_close();
	return 0;
}

/* vim: set sw=4 ts=4 expandtab : */
