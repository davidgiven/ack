/*
 * (c) copyright 1983 by the Vrije Universiteit, Amsterdam, The Netherlands.
 *
 *          This product is part of the Amsterdam Compiler Kit.
 *
 * Permission to use, sell, duplicate or disclose this software must be
 * obtained in writing. Requests for such permissions may be sent to
 *
 *      Dr. Andrew S. Tanenbaum
 *      Wiskundig Seminarium
 *      Vrije Universiteit
 *      Postbox 7161
 *      1007 MC Amsterdam
 *      The Netherlands
 *
 */

#include <stdio.h>
#include <ctype.h>

char *fname = 0 ;
char dname[200] ;
char *tail ;

FILE *intab ;
FILE *dmach ;

int index ;

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
	index=0 ;
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
	strcpy(tail,file) ;
	return fopen(dname,"r") ;
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
	fprintf(dmach,"\t{\"%s\",\t%d\t},\n",fname,index) ;
	fprintf(intab,"\n/* %s */\n\t",fname) ;
	for (;;) {
		token=getc(in) ;
		index++ ;
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
