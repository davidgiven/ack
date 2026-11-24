/* Copyright (c) 1991 by the Vrije Universiteit, Amsterdam, the Netherlands.
 * For full copyright and restrictions on use see the file COPYING in the top
 * level of the LLgen tree.
 */

/*
 *  L L G E N
 *
 *  An Extended LL(1) Parser Generator
 *
 *  Author : Ceriel J.H. Jacobs
 */

/*
 * main.c
 * Contains main program, and some error message routines
 */

#include <stdlib.h>
#include <string.h>
# include "types.h"
# include "fileio.h"
# include "extern.h"
# include "sets.h"
# include "assert.h"

# ifndef NORCSID
static string rcsid6 = "$Id$";
# endif

/* In this file the following routines are defined: */
static void readgrammar(int, char *[]);
static void doparse(p_file);
static NORETURN void comfatal(void);

int main(int argc, string argv[])
{
	string arg;

	f_dir = ".";
	f_temp = maketempfile();

	/* Initialize */

	assval = 0400;
	/* read options */

	while (argc >= 2 && (arg = argv[1], *arg == '-'))
	{
		while (*++arg)
		{
			switch (*arg)
			{
			case 'j':
			case 'J':
				jmptable_option = 1;
				if (*++arg)
					min_cases_for_jmptable = atoi(arg);
				break;
			case 'w':
			case 'W':
				wflag = 1;
				continue;
			case 'v':
			case 'V':
				verbose++;
				continue;
			case 'l':
			case 'L':
				low_percentage = atoi(++arg);
				break;
			case 'h':
			case 'H':
				high_percentage = atoi(++arg);
				break;
# ifndef NDEBUG
			case 'd':
			case 'D':
				debug++;
				continue;
			case 'r':
			case 'R':
				if (rec_file)
				{
					fprintf(stderr, "duplicate -r flag\n");
					exit(1);
				}
				rec_file = ++arg;
				break;
			case 'i':
			case 'I':
				if (incl_file)
				{
					fprintf(stderr, "duplicate -i flag\n");
					exit(1);
				}
				incl_file = ++arg;
				break;
#endif /* not NDEBUG */
			case 'x':
			case 'X':
				ntneeded = 1;
				ntprint = 1;
				continue;
#ifdef NON_CORRECTING
				case 'n':
				case 'N':
				non_corr = 1;
				continue;
				case 's':
				case 'S':
				subpars_sim = 1;
				continue;
#endif
			case 'g':
			case 'G':
				strip_grammar = 1;
				continue;

			case 'o':
			case 'O':
				f_dir = ++arg;
				break;

			default:
				fprintf(stderr, "illegal option : %c\n", *arg);
				exit(1);
			}
			break;
		}
		argv++;
		argc--;
	}

#ifdef NON_CORRECTING
	if ((subpars_sim) && (!non_corr))
	{
		fprintf(stderr,"option -s illegal without -n, turned off\n");
		subpars_sim = 0;
	}
#endif

	/*
	 * Now check wether the sets should include nonterminals
	 */
	if (verbose == 2)
		ntneeded = 1;
	/*
	 * Initialise
	 */
# ifndef NDEBUG
	if (!rec_file)
	{
# endif
		rec_file = libpath("rec");
# ifndef NDEBUG
	}
	if (!incl_file)
	{
# endif
		incl_file = libpath("incl");
# ifndef NDEBUG
	}
# endif
#ifdef NON_CORRECTING
	if (non_corr)
	{
		nc_incl_file = libpath("nc_incl");
		nc_rec_file = libpath ("nc_rec");
	}
#endif
	if ((fact = fopen(f_temp, "w")) == NULL)
	{
		fputs("Cannot create temporary\n", stderr);
		exit(1);
	}
	name_init();
	readgrammar(argc, argv);
	f_out = aprintf(OUTFILE, f_dir, prefix ? prefix : "LL");

	/* for the following two filenames only one L is used; historical
	 reasons ...
	 */
	f_include = aprintf(HFILE, f_dir, prefix ? prefix : "L");
	f_rec = aprintf(RFILE, f_dir, prefix ? prefix : "L");
#ifdef NON_CORRECTING
	if (non_corr)
		f_nc = aprintf(NCFILE, f_dir, prefix ? prefix : "L");
#endif
	setinit(ntneeded);
	maxnt = &nonterms[nnonterms];
	maxt = &tokens[ntokens];
	/*
	 * Now, the grammar is read. Do some computations
	 */
	co_reach(); /* Check for undefined and unreachable */
	if (nerrors)
		comfatal();
	do_compute();
	conflchecks();
	if (nerrors)
		comfatal();
	fclose(fact);
	fact = NULL;
	if (argc-- == 1)
	{
		fputs("No code generation for input from standard input\n",
		stderr);
	}
	else
		gencode(argc);
	remove(f_temp);
	if (verbose)
	{
		fprintf(stderr, "number of nonterminals: %d\n", nnonterms);
		fprintf(stderr, "number of tokens: %d\n", ntokens);
		fprintf(stderr, "number of term structures: %d\n", nterms);
		fprintf(stderr, "number of alternation structures: %d\n", nalts);
	}
	exit(EXIT_SUCCESS);
}

static void readgrammar(int argc, char *argv[])
{
	/*
	 * Do just what the name suggests : read the grammar
	 */
	p_file p;

	linecount = 0;
	f_input = "no filename";
	/*
	 * Build the file structure
	 */
	files = p = (p_file) alloc((argc + 1) * sizeof(t_file));
	if (argc-- == 1)
	{
		finput = stdin;
		f_input = "standard input";
		doparse(p++);
	}
	else
	{
		while (argc--)
		{
			if ((finput = fopen(f_input = argv[1], "r")) == NULL)
			{
				fatal(0, e_noopen, f_input);
			}
			doparse(p++);
			argv++;
			fclose(finput);
		}
	}
	maxfiles = p;
	if (!lexical)
		lexical = "yylex";
	/*
	 * There must be a start symbol!
	 */
	if (!nerrors && start == 0)
	{
		fatal(linecount, "Missing %%start", NULL);
	}
	if (nerrors)
		comfatal();
}

static void doparse(p_file p)
{
	linecount = 0;
	p->f_name = f_input;
	p->f_firsts = 0;
	pfile = p;
	torder = -1;
	norder = -1;
	LLparse();
	p->f_nonterminals = norder;
	p->f_terminals = torder;
}

void error(int lineno, string s, string t)
{
	/*
	 * Just an error message
	 */

	++nerrors;
	if (!lineno)
		lineno = 1;
	fprintf(stderr, "\"%s\", line %d: ", f_input, lineno);
	fprintf(stderr, s, t);
	fputs("\n", stderr);
}

void warning(int lineno, string s, string t)
{
	/*
	 * Just a warning
	 */

	if (wflag)
		return;
	if (!lineno)
		lineno = 1;
	fprintf(stderr, "\"%s\", line %d: (Warning) ", f_input, lineno);
	fprintf(stderr, s, t);
	fputs("\n", stderr);
}

void fatal(int lineno, string s, string t)
{
	/*
	 * Fatal error
	 */
	error(lineno, s, t);
	comfatal();
}

static NORETURN void comfatal(void)
{
	/*
	 * Some common code for exit on errors
	 */
	if (fact != NULL)
	{
		fclose(fact);
	}
	if (f_temp != NULL)
		remove(f_temp);
	if (fpars != NULL)
		fclose(fpars);
	if (fout != NULL)
		fclose(fout);
	exit(EXIT_FAILURE);
}

void copyfile(string file)
{
	/*
	 * Copies a file indicated by the parameter to filedescriptor fpars.
	 */
	int c;
	FILE *f;

	if ((f = fopen(file, "r")) == NULL)
	{
		fatal(0, "Cannot open library file '%s', call an expert", file);
	}
	while ((c = getc(f)) != EOF)
		putc(c, fpars);
	fclose(f);
}

FILE* openoutput(string filename)
{
	FILE* f = fopen(filename, "w");
	if (f == NULL)
		fatal(0, "Cannot open output file '%s', call an expert", filename);
	fprintf(stderr, "%s\n", filename);
	return f;
}

void closeoutput(FILE** f)
{
	int failed = ferror(*f);
	if (fclose(*f) != 0)
		failed = 1;
	*f = NULL;
	if (failed)
		fatal(0, "Write error on output", NULL);
}
