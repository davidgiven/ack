#ifndef NORCSID
static char rcsid[] = "$Header$";
#endif

#include "param.h"

/*
 * (c) copyright 1983 by the Vrije Universiteit, Amsterdam, The Netherlands.
 *
 *          This product is part of the Amsterdam Compiler Kit.
 *
 * Permission to use, sell, duplicate or disclose this software must be
 * obtained in writing. Requests for such permissions may be sent to
 *
 *      Dr. Andrew S. Tanenbaum
 *      Wiskundig Seminarium
 *      Vrije Universiteit
 *      Postbox 7161
 *      1007 MC Amsterdam
 *      The Netherlands
 *
 * Author: Hans van Staveren
 */

char *progname;
extern char startupcode[];
int maxply=1;
#ifndef NDEBUG
int Debug=0;
#endif

extern int endofprog;

main(argc,argv) char **argv; {
	register unsigned n;
	extern unsigned cc1,cc2,cc3,cc4;
	unsigned ggd();

	progname = argv[0];
	while (--argc && **++argv == '-') {
		switch(argv[0][1]) {
#ifndef NDEBUG
		case 'd':
			Debug=1; break;
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
			error("Unknown flag %c",argv[0][1]);
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

unsigned ggd(a,b) register unsigned a,b; {
	register unsigned c;

	do {
		c = a%b; a=b; b=c;
	} while (c!=0);
	return(a);
}
