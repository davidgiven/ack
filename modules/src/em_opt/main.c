/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
#ifndef NORCSID
static char rcsid1[] = "$Header$";
#endif

/*	This is the main program for the stand-alone version of the
	peephole optimizer.
*/

#include	"nopt.h"

char *filename;			/* Name of input file */
int errors;			/* Number of errors */

main(argc,argv)
	char **argv;
{
	register p_instr p = GETINSTR();

	if (argc >= 2) {
		filename = argv[1];
	}
	else	filename = 0;
	if (!EM_open(filename)) {
		fatal(EM_error);
	}
	EM_getinstr(p);
	O_init((arith) EM_wordsize, (arith) EM_pointersize);
	if (argc >= 3) {
		if (!O_open(argv[2])) {
			fatal("O_open failed");
		}
	}
	else	if (!O_open( (char *) 0)) fatal("O_open failed");
	O_magic();
	for(;;) {
		switch(p->em_type) {
		case EM_DEFILB:
			*OO_nxtpatt++ = p;
			OO_dfa(p->em_opcode=op_lab);
			break;
		case EM_MNEM:
			switch(p->em_argtype) {
			case sof_ptyp:
				p->em_dnam = OO_freestr(p->em_dnam);
				break;
			case pro_ptyp:
				p->em_pnam = OO_freestr(p->em_pnam);
				break;
			case str_ptyp:
			case ico_ptyp:
			case uco_ptyp:
			case fco_ptyp:
				p->em_string = OO_freestr(p->em_string);
				break;
			}
			*OO_nxtpatt++ = p;
			OO_dfa(p->em_opcode);
			break;
		default:
			FLUSHDFA();
			EM_mkcalls(p);
			OO_free(p);
			break;
		case EM_EOF:
			goto got_eof;
		case EM_ERROR:
			error("%s", EM_error);
			break;
		case EM_FATAL:
			fatal("%s", EM_error);
		}
		EM_getinstr(p=GETINSTR());
	}
 got_eof:
	O_close();
	EM_close();
	exit(errors);
}

/*VARARGS1*/
error(s,a1,a2,a3,a4)
	char *s;
{
	fprintf(stderr,
		"%s, line %d: ",
		filename ? filename : "standard input",
		EM_lineno);
	fprintf(stderr,s,a1,a2,a3,a4);
	fprintf(stderr, "\n");
	errors++;
}

/*VARARGS1*/
fatal(s,a1,a2,a3,a4)
	char *s;
{
	error(s,a1,a2,a3,a4);
	exit(1);
}
