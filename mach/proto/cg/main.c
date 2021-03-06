/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 *
 * Author: Hans van Staveren
 */
#include <stdlib.h>
#include "tables.h"
#include "types.h"
#include <cg_pattern.h>
#include "data.h"
#include "tables.h"
#include "param.h"
#include "mach.h"
#include "subr.h"
#include "fillem.h"
#include "gencode.h"
#include "codegen.h"

char *progname;
extern byte startupcode[];
int maxply=1;
#ifndef NDEBUG
int Debug=0;
#endif

extern int endofprog;


unsigned ggd(unsigned int a,unsigned int b)
{
	register unsigned c;

	do {
		c = a%b; a=b; b=c;
	} while (c!=0);
	return(a);
}


int main(int argc, char **argv)
{
	register unsigned n;
	extern unsigned cc1,cc2,cc3,cc4;


	progname = argv[0];
	while (--argc && **++argv == '-') {
		switch(argv[0][1]) {
#ifndef NDEBUG
		case 'd':
			Debug++; break;
#endif
		case 'p':
			maxply = atoi(argv[0]+2);
			break;
		case 'w':	/* weight percentage for size */
			n=atoi(argv[0]+2);
			cc1 *= n;
			cc2 *= 50;
			cc3 *= (100-n);
			cc4 *= 50;
			n=ggd(cc1,cc2);
			cc1 /= n;
			cc2 /= n;
			n=ggd(cc3,cc4);
			cc3 /= n;
			cc4 /= n;
			break;
		default:
#ifdef MACH_OPTIONS
			mach_option(argv[0]);
#else
			error("Unknown flag %c",argv[0][1]);
#endif
			break;
		}
	}
	if (argc < 1 || argc > 2)
		error("Usage: %s EMfile [ asfile ]",progname);
	in_init(argv[0]);
	out_init(argv[1]);
	in_start();
	codegen(startupcode,maxply,TRUE,MAXINT,0);
	in_finish();
	if (!endofprog)
		error("Bombed out of codegen");
	out_finish();
}

