/* $Header$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* MAIN PROGRAM */

#include "file_info.h"
#include "idfsize.h"

extern char *symbol2str();
extern char *getwdir();
extern char *inctable[];
extern int err_occurred;
int idfsize = IDFSIZE;

int ifval;

char *prog_name;

main(argc, argv)
	char *argv[];
{
	/* parse and interpret the command line options	*/
	prog_name = argv[0];

	init_idf();
	init_pp();	/* initialise the preprocessor macros	*/

	/*	Note: source file "-" indicates that the source is supplied
		as standard input.  This is only allowed if INP_READ_IN_ONE is
		not defined!
	*/
	while (argc > 1 && *argv[1] == '-' && argv[1][1] != '\0')	{
		char *par = &argv[1][1];

		if (*par == '-')
			par++;
		do_option(par);
		argc--, argv++;
	}
	compile(argc - 1, &argv[1]);
	exit(err_occurred);
}

compile(argc, argv)
	char *argv[];
{
	register char *source = 0;
	char *dummy;

	switch (argc) {
	case 1:
		source = argv[0];
		FileName = source;
		break;
	case 0:
		FileName = "";
		WorkingDir = 0;
		break;
	default:
		fatal("use: %s [options] [source]", prog_name);
		break;
	}

	if (!InsertFile(source, (char **) 0, &dummy)) /* read the source file	*/
		fatal("%s: no source file %s\n", prog_name, 
			source ? source : "stdin");
	if (source) WorkingDir = getwdir(dummy);
	preprocess(source);
}
