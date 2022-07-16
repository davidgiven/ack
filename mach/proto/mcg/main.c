#include "mcg.h"
#include <errno.h>
#include <unistd.h>

static const char* tracechars = NULL;

FILE* outputfile = NULL;
FILE* dominance_dot_file = NULL;
FILE* cfg_dot_file = NULL;

bool tracing(char k)
{
    if (k == '!')
        return true;
    if (!tracechars)
        return false;

    return strchr(tracechars, k);
}

void tracef(char k, const char* fmt, ...)
{
    va_list ap;

    if (tracing(k))
    {
        va_start(ap, fmt);
        vfprintf(stderr, fmt, ap);
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
    const char* inputfilename = NULL;
    const char* outputfilename = NULL;
    FILE* output;
    int i;

    program_name = argv[0];

    opterr = 1;
    for (;;)
    {
        int c = getopt(argc, argv, "d:D:C:o:");
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

            case 'o':
                if (outputfilename)
                    fatal("already specified an output file");
                outputfilename = optarg;
                break;
        }
    }

    for (i = optind; i < argc; i++)
    {
        if (inputfilename)
            fatal("unexpected argument '%s'", argv[i]);
        inputfilename = argv[i];
    }

    symbol_init();

    if (!EM_open((char*) inputfilename))
        fatal("couldn't open input '%s': %s",
            inputfilename ? inputfilename : "<stdin>", EM_error);

    if (outputfilename)
    {
        outputfile = fopen(outputfilename, "w");
        if (!outputfile)
            fatal("couldn't open output '%s': %s",
                outputfilename, strerror(errno));
    }
    else
        outputfile = stdout;

    fprintf(outputfile, ".sect .text\n.sect .rom\n.sect .data\n.sect .bss\n");

    /* Reads in the EM, outputs the data sections, parses any code and
     * generates IR trees. */

    parse_em();

    /* For every procedure, go ahead and do the compilation proper. We do this
     * now so that we know that all the data has been read correctly and our
     * symbol table is complete (we may need to refer to it). */

    symbol_walk(find_procedures_cb, NULL);

    if (outputfilename)
        fclose(outputfile);
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
