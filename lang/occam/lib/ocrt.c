/* $Id$ */
/*	ocrt.c - Occam runtime support */
#include "ocm_chan.h"

int chandes[]= { 0, 0, sizeof(int)+sizeof(long) };
int worddes[]= { 0, 0, sizeof(long) };
int bytedes[]= { 0, 0, 1 };
long any;

void catch(sig, file, line) int sig; char *file; int line;
/* Catches traps in the occam program */
{
	register char *mes;

	switch (sig) {
	case 0:
		mes="array bound error";
		break;
	case 6:
		mes="division by zero";
		break;
	case 8:
		mes="undefined variable";
		break;
	default:
		return;
	}
	fprintf(stderr, "%s (%d) F: %s\n", file, line, mes);
	abort();
}

chan file[20];
FILE *unix_file[20];

void initfile()
{
	register i;
	register chan *c=file;

	for (i=0; i<20; i++) {
		c->type=C_T_FILE;
		c->f.flgs=0;
		(c++)->f.index=i;
	}
	file[0].f.flgs|=C_F_INUSE;
	unix_file[0]=stdin;

	file[1].f.flgs|=C_F_INUSE;
	unix_file[1]=stdout;

	file[2].f.flgs|=C_F_INUSE;
	unix_file[2]=stderr;
}
