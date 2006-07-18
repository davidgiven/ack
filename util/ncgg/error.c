/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
#ifndef NORCSID
static char rcsid[]= "$Id$";
#endif

#include <stdlib.h>
#include <stdio.h>

int nerrors=0;

yyerror(s) char *s; {

	error("Parser gives %s",s);
}

goodbye() {

	error("This was fatal, goodbye!");
#ifndef NDEBUG
	abort();
#endif
}

/*VARARGS1*/
fatal(s,a,b,c,d) char *s; {

	error(s,a,b,c,d);
	errorexit();
	goodbye();
	exit(-1);
}

/*VARARGS1*/
error(s,a,b,c,d) char *s; {
	extern int lineno;
	extern char *filename;

	fprintf(stderr,"\"%s\", line %d:",filename,lineno);
	fprintf(stderr,s,a,b,c,d);
	fprintf(stderr,"\n");
	nerrors++;
}

#ifndef NDEBUG
badassertion(string,file,line) char *string,*file; {

	fprintf(stderr,"\"%s\", line %d: Assertion failed \"%s\"\n",
		file,line,string);
	goodbye();
}
#endif

tabovf(string) char *string; {

	fatal("%s overflow",string);
}
