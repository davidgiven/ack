/* $Header$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* MAIN PROGRAM */

#include <alloc.h>
#include <em_arith.h>
#include <assert.h>
#include "file_info.h"
#include "idfsize.h"
#include "mkdep.h"
#ifdef MKDEP
#include "idf.h"
#include "macro.h"
#endif

extern char *symbol2str();
extern char *getwdir();
extern int err_occurred;
int idfsize = IDFSIZE;
extern char options[];

arith ifval;

char *prog_name;

extern char **inctable;
extern int inc_max, inc_total;

main(argc, argv)
	char *argv[];
{
	/* parse and interpret the command line options	*/
	prog_name = argv[0];

	init_idf();

	inctable = (char **) Malloc(10 * sizeof(char *));
	inc_max = 10;
	inc_total = 3;
	inctable[0] = "";
	inctable[1] = "/usr/include";
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
#ifdef MKDEP
	options['P'] = 1;
#endif
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
		FileName = argv[0];
		fatal("use: %s [options] [source]", prog_name);
		break;
	}

	if (!InsertFile(source, (char **) 0, &dummy)) /* read the source file	*/
		fatal("%s: no source file %s\n", prog_name, 
			source ? source : "stdin");
	if (source) WorkingDir = getwdir(dummy);
	preprocess(source);
#ifdef MKDEP
	list_dependencies(source);
#endif
}

#ifdef MKDEP
struct idf	*file_head;
extern char *strrindex();

list_dependencies(source)
	char *source;
{
	register struct idf *p = file_head;

	if (source) {
		register char *s = strrindex(source, '.');

		if (s && *(s+1)) {
			s++;
			*s++ = 'o';
			*s = '\0';
		}
		else source = 0; 
	}
	while (p) {
		assert(p->id_resmac == K_FILE);
		dependency(p->id_text, source);
		p = p->id_file;
	}
}

add_dependency(s)
	char *s;
{
	register struct idf *p = str2idf(s, 0);

	if (! p->id_resmac) {
		p->id_resmac = K_FILE;
		p->id_file = file_head;
		file_head = p;
	}
}

dependency(s, source)
	char *s;
{
	if (options['s'] && !strncmp(s, "/usr/include/", 13)) {
		return;
	}
	if (options['m'] && source) print("%s: ", source);
	print("%s\n", s);
}
#endif
