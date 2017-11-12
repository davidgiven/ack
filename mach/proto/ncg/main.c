#ifndef NORCSID
static char rcsid[] = "$Id$";
#endif

#include "param.h"
#include "tables.h"
#include "mach.h"

/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 *
 * Author: Hans van Staveren
 */

char *progname;
extern char startupcode[];
extern unsigned codegen();
int maxply=1;
#ifndef NDEBUG
int Debug=0;
char *strtdebug="";
#endif

/* fillem.c */
void in_init(char *);
void in_start(void);

main(argc,argv) char **argv; {
	register unsigned n;
	extern unsigned cc1,cc2,cc3,cc4;
	unsigned ggd();

	progname = argv[0];
	while (--argc && **++argv == '-') {
		switch(argv[0][1]) {
#ifndef NDEBUG
		case 'd':
			if ((Debug = argv[0][2]) != 0) {
				Debug -= '0';
				if (argv[0][3] == '@') {
					Debug = 0;
					strtdebug = &argv[0][4];
				}
			} else
				Debug++;
			break;
#endif
#ifdef TABLEDEBUG
		case 'u':
		case 'U':
			initlset(argv[0]+1);
			break;
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
	if (argc > 2)
		error("Usage: %s [ EMfile ] [ asfile ]",progname);
	in_init(argc >= 1 ? argv[0] : (char *) 0);
	out_init(argc >= 2 ? argv[1] : (char *) 0);
	readcodebytes();
	itokcost();
	in_start();
	codegen(startupcode,maxply,TRUE,MAXINT,0);
	error("Bombed out of codegen");
}

unsigned ggd(a,b) register unsigned a,b; {
	register unsigned c;

	do {
		c = a%b; a=b; b=c;
	} while (c!=0);
	return(a);
}
