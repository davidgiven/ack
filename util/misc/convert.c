#ifndef NORCSID
static char rcsid[] = "$Header$";
#endif

/*	This program converts either human-readable or compact EM
	assembly code to calls of the procedure-interface.
	It must be linked with two libraries:
	- a library to read EM code, according to read_em(3)
	- a library implementing the em_code(3) interface.
	Thus, this program could serve as an EM_encoder, an
	EM_decoder, or some code generator, depending on how it is
	linked.
*/

#include	<stdio.h>
#include        <em_pseu.h>
#include	<em_mnem.h>
#include        <em_spec.h>
#include        <em_flag.h>
#include        <em_ptyp.h>
#include	<em.h>
#include	<em_comp.h>

char *filename;			/* Name of input file */
int errors;			/* Number of errors */

main(argc,argv)
	char **argv;
{
	struct e_instr *EM_getinstr();
	register struct e_instr *p;
	register struct e_args *ap;

	if (argc >= 2) {
		filename = argv[1];
	}
	else	filename = 0;
	if (!EM_open(filename)) {
		fatal(EM_error);
	}
	p = EM_getinstr();
	C_init((arith) EM_wordsize, (arith) EM_pointersize);
	if (argc >= 3) {
		if (!C_open(argv[2])) {
			fatal("C_open failed");
		}
	}
	else	if (!C_open( (char *) 0)) fatal("C_open failed");
	C_magic();
	while (p) {
		if (p->em_type == EM_FATAL) {
			fatal("%s", EM_error);
		}
		if (p->em_type == EM_ERROR) {
			error("%s", EM_error);
		}
		else if (!EM_mkcalls(p)) {
			error("%s", EM_error);
		}
		p = EM_getinstr();
	}
	C_close();
	EM_close();
	exit(errors);
}

/* VARARGS */
error(s,a1,a2,a3,a4)
	char *s;
{
	fprintf(stderr,
		"%s, line %d: ",
		filename ? filename : "standard input",
		EM_lineno);
	fprintf(stderr,s,a1,a2,a3,a4);
	putc('\n', stderr);
	errors++;
}

/* VARARGS */
fatal(s,a1,a2,a3,a4)
	char *s;
{
	error(s,a1,a2,a3,a4);
	exit(1);
}
