/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 *
 * Author: Hans van Staveren
 */
#include <stdlib.h>
#include <stdio.h>
#include "param.h"
#include "types.h"
#include "tes.h"
#include "alloc.h"
#include "system.h"
#include <em_spec.h>
#include "ext.h"
#include "util.h"
#include "getline.h"
#include "putline.h"

/* Other external definitions */
extern void cleanup(void);

void flags(register char *s)
{
	for (s++; *s; s++)
		switch (*s)
		{
			case 'L':
				Lflag = TRUE;
				break;
			case 'n':
				nflag = TRUE;
				break;
			case 'm':
				if (*(s + 1) == 'l')
				{
					s++;
					repl_longmuls = TRUE;
				}
				repl_muls = atoi(s + 1);
				break;
		}
}

void fileinit(void)
{
	if (readshort() != (short) sp_magic)
		error("wrong input file");
	if (Lflag)
	{

		if (sys_tmpnam(tempname)==NULL)
		{
			error("can't create temporary file.");
		}
		outfile = fopen(tempname, "wb");
		if (outfile == NULL)
			error("can't create %s", tempname);
	}
	else
	{
		sys_setbinarymode(stdout);
		outfile = stdout;
		outshort(sp_magic);
	}
}


/*
 * Main program for EM optimizer
 */

int main(int argc, char* argv[])
{
#ifndef USEMALLOC
	int somespace[STACKROOM];
#endif

	progname = argv[0];
	while (argc-- > 1 && **++argv == '-')
		flags(*argv);
	if (argc > 1)
	{
		fprintf(stderr, "Usage: %s [-Ln] [-m<num>] [name]\n", progname);
		exit(EXIT_FAILURE);
	}
	if (argc)
	{
		if (freopen(*argv, "rb", stdin) == NULL)
			error("Cannot open %s", *argv);
	}
	else
		sys_setbinarymode(stdin);

	fileinit();
#ifdef USEMALLOC
	coreinit();
#else
	coreinit((short *) somespace, (short *) (somespace + STACKROOM));
#endif
	getlines();
	cleanup();
	return (EXIT_SUCCESS);
}

