/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#ifndef NORCSID
static char rcs_id[] = "$Id$" ;
#endif

char *fname = 0 ;
char dname[200] ;
char *tail ;

FILE *intab ;
FILE *dmach ;

int offset ;

main(argc,argv) char **argv ; {
	register i ;

	start(argv[1]) ;
	for ( i=2 ; i<argc ; i++ ) {
		fname= argv[i] ;
		readm() ;
	}
	stop(argc>2) ;
	return 0 ;
}

start(dir) char *dir ; {
	tail= dname ;
	while ( *dir ) {
		*tail++ = *dir ++ ;
	}
	if ( tail!=dname ) *tail++= '/' ;
	offset=0 ;
	intab= fopen("intable.c","w");
	dmach= fopen("dmach.c","w");
	if ( intab==NULL || dmach==NULL ) {
		fprintf(stderr,"Couln't create output file(s)\n");
		exit ( 1) ;
	}
	fprintf(dmach,"#include \"dmach.h\"\n\ndmach\tmassoc[] = {\n") ;
	fprintf(intab,"char intable[] = {\n") ;
}

stop(filled) {
	fprintf(dmach,"\t{\"\",\t-1\t}\n} ;\n") ;
	if ( !filled ) fprintf(intab,"\t0\n") ;
	fprintf(intab,"\n} ;\n") ;
	fclose(dmach); fclose(intab) ;
}

FILE *do_open(file) char *file ; {
	FILE *fd;

	strcpy(tail,file) ;
	strcat(tail,"/");
	strcat(tail,"descr");
	if ((fd = fopen(dname,"r")) != NULL) return fd;
	strcpy(tail,"descr/");
	strcat(tail,file);
	return fopen(dname,"r");
}

readm() {
	register int i ;
	register int token ;
	register FILE *in ;

	in=do_open(fname) ;
	if ( in==NULL ) {
		fprintf(stderr,"Cannot open %s\n",fname) ;
		return ;
	}
	i=0 ;
	fprintf(dmach,"\t{\"%s\",\t%d\t},\n",fname,offset) ;
	fprintf(intab,"\n/* %s */\n\t",fname) ;
	for (;;) {
		token=getc(in) ;
		offset++ ;
		if ( ++i == 10 ) {
			fprintf(intab,"\n\t") ;
			i=0 ;
		} else {
			fprintf(intab," ") ;
		}
		if ( !isascii(token) || !(isprint(token) || isspace(token)) ){
			if ( token!=EOF ) {
			  fprintf(stderr,"warning: non-ascii in %s\n",fname) ;
			  fprintf(intab,"%4d,",token) ;
			} else {
			  fprintf(intab,"  0,",token) ;
			  break ;
			}
		} else if ( isprint(token) ) {
			switch ( token ) {
			case '\'': fprintf(intab,"'\\''") ; break ;
			case '\\': fprintf(intab,"'\\\\'") ; break ;
			default:   fprintf(intab," '%c'",token) ; break ;
			}
		} else switch ( token ) {
		case '\n' : fprintf(intab,"'\\n'") ; break ;
		case '\t' : fprintf(intab,"'\\t'") ; break ;
		case '\r' : fprintf(intab,"'\\r'") ; break ;
		case '\f' : fprintf(intab,"'\\f'") ; break ;
		case ' '  : fprintf(intab," ' '")  ; break ;
		default :   fprintf(stderr,"warning: unrec. %d\n",
				token) ;
			    fprintf(intab,"%4d",token) ;
			    break ;
		}
		fprintf(intab,",") ;
	}
	fclose(in) ;
}
