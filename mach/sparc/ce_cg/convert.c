/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
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

#include	<system.h>
#include        <em_pseu.h>
#include	<em_mnem.h>
#include        <em_spec.h>
#include        <em_flag.h>
#include        <em_ptyp.h>
#include	<em.h>
#include	<em_comp.h>

char *filename;			/* Name of input file */
char *out_file;			/* Name of output file */
int errors;			/* Number of errors */
int debug;
extern char *C_error;

main(argc,argv)
	char **argv;
{
	struct e_instr buf;
	register struct e_instr *p = &buf;
	int ac;
	char **av;

	filename= 0;
	out_file= 0;
	debug= 0;
	for (ac=argc-1,av=argv+1;ac;ac--,av++)
	{
		if (av[0][0]=='-' && av[0][1]=='d' && av[0][2]==0)
		{
			debug= 1;
			continue;
		}
		if (!filename)
		{
			if (strcmp(*av, "-"))
				filename= *av;
		}
		else if (!out_file)
		{
			if (strcmp(*av, "-"))
				out_file= *av;
		}
		else
			fatal ("too many arguments");
	}
	if (!EM_open(filename)) {
		fatal(EM_error);
	}
	EM_getinstr(p);
	C_init((arith) EM_wordsize, (arith) EM_pointersize);
	if (!C_open(out_file)) {
		fatal("C_open failed");
	}
	C_magic();
	while (p->em_type != EM_EOF) {
		if (p->em_type == EM_FATAL) {
			fatal("%s", EM_error);
		}
		if (EM_error) {
			error("%s", EM_error);
		}
		if (p->em_type != EM_ERROR && !C_out(p)) {
			error("%s", C_error);
		}
		EM_getinstr(p);
	}
	C_close();
	EM_close();
	exit(errors);
}

/* VARARGS */
error(s,a1,a2,a3,a4)
	char *s;
{
	fprint(STDERR,
		"%s, line %d: ",
		filename ? filename : "standard input",
		EM_lineno);
	fprint(STDERR,s,a1,a2,a3,a4);
	fprint(STDERR, "\n");
	errors++;
}

/* VARARGS */
fatal(s,a1,a2,a3,a4)
	char *s;
{
	if (C_busy()) C_close();
	error(s,a1,a2,a3,a4);
	exit(1);
}
