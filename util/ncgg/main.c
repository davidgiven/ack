#ifndef NORCSID
static char rcsid[]= "$Header$";
#endif

#include <stdio.h>

char *filename;

main(argc,argv) char **argv; {
	extern int nerrors;
	extern int code_in_c;
	extern int tabledebug;
	extern int verbose;

	while (argc >1 && argv[1][0]=='-') {
		switch(argv[1][1]) {
		case 'c':
			code_in_c = 0;
			break;
		case 'd':
			tabledebug++;
			break;
		case 'v':
			verbose++;
			break;
		default:
			error("Unknown flag -%c",argv[1][1]);
		}
		argc--; argv++;
	}
	if (argc==2) {
		if (freopen(argv[1],"r",stdin)==NULL) {
			error("Can't open %s",argv[1]);
			exit(-1);
		}
		filename = argv[1];
	}
	else if (argc == 1) {
		filename = "";
	} else
		error("Usage: %s [-c] [-d] [-v] [table]",argv[0]);
	initemhash();
	enterkeyw();
	initnodes();
	initio();
	yyparse();
	if (nerrors==0) {
		finishio();
		statistics();
		if (verbose)
			hallverbose();
	} else {
		errorexit();
	}
	return(nerrors==0 ? 0 : -1);
}
