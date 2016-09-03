/* $Id$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* m a i n . c
 *
 * Contains the main program, the error reporting routine, and a routine
 * to check wether a constraint consists only of space
 */
 
#include <stdlib.h>
#include <stdio.h>

extern int lineno, newline;

FILE *genc, *genh, *input;
static int nerrors;
char *linedir = "#line %d \"%s\"\n";	/* format of line directive */
char *inpfile;

main(argc,argv) char *argv[]; {

    newline = 1;
    if (argc != 3) {
	fprintf(stderr,"Usage : %s targetoptimizerdescription outputdir\n",argv[0]);
	exit(1);
    }
    if ((input = fopen(argv[1],"r")) == NULL) {
	fprintf(stderr,"Fatal error : couldn't open %s\n",argv[1]);
	exit(1);
    }
	if (chdir(argv[2]) != 0) {
	fprintf(stderr,"Fatal error : couldn't chdir to %s\n",argv[2]);
	exit(1);
    }
    if ((genc = fopen("gen.c","w")) == NULL) {
	fputs("Fatal error : couldn't open gen.c\n",stderr);
	exit(1);
    }
    if ((genh = fopen("gen.h","w")) == NULL) {
	fputs("Fatal error : couldn't open gen.h\n",stderr);
	exit(1);
    }
    inpfile = argv[1];		/* needed for line directives and errors */
    LLparse();
    exit(nerrors);
}

/* VARARGS1 */
error(s, s1) char *s, *s1; {
    
    nerrors++;
    fprintf(stderr,"\"%s\", line %d: ",inpfile,lineno);
    fprintf(stderr,s,s1);
    putc('\n',stderr);
}

onlyspace(s) register char *s; {
    
    while (*s) {
	if (*s != ' ' && *s != '\t' && *s != '\n') return 0;
	s++;
    }
    return 1;
}
