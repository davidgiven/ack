#include "mcg.h"
#include <errno.h>
#include <unistd.h>

static const char* tracechars = NULL;

FILE* outputfile = NULL;
FILE* dominance_dot_file = NULL;
FILE* cfg_dot_file = NULL;
FILE* regalloc_dot_file = NULL;

bool tracing(char k)
{
    if (k == '!')
        return true;
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

static FILE* open_dot_file(const char* filename)
{
    FILE* fp = fopen(filename, "w");
    if (!fp)
        fatal("couldn't open output file '%s': %s",
            filename, strerror(errno));
    fprintf(fp, "digraph {\n");
    return fp;
}

static void close_dot_files(void)
{
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
    if (regalloc_dot_file)
    {
        fprintf(regalloc_dot_file, "}\n");
        fclose(regalloc_dot_file);
    }
}

int main(int argc, char* const argv[])
{
    const char* inputfilename = NULL;
    const char* outputfilename = NULL;
    FILE* output;

    program_name = argv[0];

    opterr = 1;
    for (;;)
    {
        int c = getopt(argc, argv, "-d:D:C:R:o:");
        if (c == -1)
            break;

        switch (c)
        {
            case 'C':
                cfg_dot_file = open_dot_file(optarg);
                break;

            case 'D':
                dominance_dot_file = open_dot_file(optarg);
                break;

            case 'R':
                regalloc_dot_file = open_dot_file(optarg);
                break;

            case 'd':
                tracechars = optarg;
                break;

            case 'o':
                if (outputfilename)
                    fatal("already specified an output file");
                outputfilename = optarg;
                break;

            case 1:
                if (inputfilename)
                    fatal("unexpected argument '%s'", optarg);
                inputfilename = optarg;
        }
    }
    atexit(close_dot_files);

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

	return 0;
}

/* vim: set sw=4 ts=4 expandtab : */
