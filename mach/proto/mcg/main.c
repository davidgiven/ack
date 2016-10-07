#include "mcg.h"
#include <errno.h>
#include <unistd.h>

static const char* tracechars = NULL;

FILE* dominance_dot_file = NULL;
FILE* cfg_dot_file = NULL;

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
    const char* inputfile = NULL;

    program_name = argv[0];

    opterr = 1;
    for (;;)
    {
        int c = getopt(argc, argv, "-d:D:C:");
        if (c == -1)
            break;

        switch (c)
        {
            case 'C':
                cfg_dot_file = fopen(optarg, "w");
                if (!cfg_dot_file)
                    fatal("couldn't open output file '%s': %s",
                        optarg, strerror(errno));
                fprintf(cfg_dot_file, "digraph {\n");
                break;

            case 'D':
                dominance_dot_file = fopen(optarg, "w");
                if (!dominance_dot_file)
                    fatal("couldn't open output file '%s': %s",
                        optarg, strerror(errno));
                fprintf(dominance_dot_file, "digraph {\n");
                break;

            case 'd':
                tracechars = optarg;
                break;

            case 1:
                if (inputfile)
                    fatal("unexpected argument '%s'", optarg);
                inputfile = optarg;
        }
    }

    symbol_init();

	if (!EM_open((char*) inputfile))
		fatal("couldn't open '%s': %s",
            inputfile ? inputfile : "<stdin>", EM_error);
	
    /* Reads in the EM, outputs the data sections, parses any code and
     * generates IR trees. */

    parse_em();

    /* For every procedure, go ahead and do the compilation proper. We do this
     * now so that we know that all the data has been read correctly and our
     * symbol table is complete (we may need to refer to it). */

    symbol_walk(find_procedures_cb, NULL);

	EM_close();
    if (cfg_dot_file)
    {
        fprintf(cfg_dot_file, "}\n");
        fclose(cfg_dot_file);
    }
    if (dominance_dot_file)
    {
        fprintf(dominance_dot_file, "}\n");
        fclose(dominance_dot_file);
    }
	return 0;
}

/* vim: set sw=4 ts=4 expandtab : */
