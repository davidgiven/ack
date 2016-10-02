#include "mcg.h"

bool tracing(char k)
{
    switch (k)
    {
        case 0:   return true;
        case 'S': return true;
        case 'E': return false;
        case 'G': return true;
        case '0': return false;
        case '1': return false;
        case '2': return false;
        case '3': return false;
        case '4': return false;
        case '5': return false;
        case 'I': return true;
        default:  return true;
    }
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

int main(int argc, char* argv[])
{
    symbol_init();

	if (!EM_open(argv[1]))
		fatal("Couldn't open input file: %s", EM_error);
	
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
