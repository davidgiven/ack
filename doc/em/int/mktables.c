/* $Id$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 *
 */

/* Author: E.G. Keizer */

#include <stdio.h>
#include <ip_spec.h>
#include <em_spec.h>
#include <em_flag.h>

/* This program reads the human readable interpreter specification
   and produces a efficient machine representation that can be
   translated by a C-compiler.
*/

#define ESCAP   256

int nerror = 0 ;
int atend  = 0 ;
int line   = 1 ;
int maxinsl= 0 ;

extern char em_mnem[][4] ;
char esca[] = "escape" ;
#define ename(no)       ((no)==ESCAP?esca:em_mnem[(no)])

extern char em_flag[] ;

main(argc,argv) char **argv ; {
	if ( argc>1 ) {
		if ( freopen(argv[1],"r",stdin)==NULL) {
			fatal("Cannot open %s",argv[1]) ;
		}
	}
	if ( argc>2 ) {
		if ( freopen(argv[2],"w",stdout)==NULL) {
			fatal("Cannot create %s",argv[2]) ;
		}
	}
	if ( argc>3 ) {
		fatal("%s [ file [ file ] ]",argv[0]) ;
	}
	atend=0 ;
	readin();
	atend=1 ;
	exit(nerror) ;
}

readin() {
	char *ident();
	char *firstid ;
	int opcode,flags;
	int c;

	while ( !feof(stdin) ) {
		firstid=ident() ;
		if ( *firstid=='\n' || feof(stdin) ) continue ;
		opcode = getmnem(firstid) ;
		printf("%d ",opcode+1) ;
		flags  = decflag(ident(),opcode) ;
		switch(em_flag[opcode]&EM_PAR) {
		case PAR_D: case PAR_F: case PAR_B: case PAR_L: case PAR_C:
			putchar('S') ;
		}
		putchar(' ');
		while ( (c=readchar())!='\n' && c!=EOF ) putchar(c) ;
		putchar('\n') ;
	}
}

char *ident() {
	/* skip spaces and tabs, anything up to space,tab or eof is
	   a identifier.
	   Anything from # to end-of-line is an end-of-line.
	   End-of-line is an identifier all by itself.
	*/

	static char array[200] ;
	register int c ;
	register char *cc ;

	do {
		c=readchar() ;
	} while ( c==' ' || c=='\t' ) ;
	for ( cc=array ; cc<&array[(sizeof array) - 1] ; cc++ ) {
		if ( c=='#' ) {
			do {
				c=readchar();
			} while ( c!='\n' && c!=EOF ) ;
		}
		*cc = c ;
		if ( c=='\n' && cc==array ) break ;
		c=readchar() ;
		if ( c=='\n' ) {
			pushback(c) ;
			break ;
		}
		if ( c==' ' || c=='\t' || c==EOF ) break ;
	}
	*++cc=0 ;
	return array ;
}

int getmnem(str) char *str ; {
	char (*ptr)[4] ;

	for ( ptr = em_mnem ; *ptr<= &em_mnem[sp_lmnem][0] ; ptr++ ) {
		if ( strcmp(*ptr,str)==0 ) return (ptr-em_mnem) ;
	}
	error("Illegal mnemonic") ;
	return 0 ;
}

error(str,a1,a2,a3,a4,a5,a6) /* VARARGS1 */ char *str ; {
	if ( !atend ) fprintf(stderr,"line %d: ",line) ;
	fprintf(stderr,str,a1,a2,a3,a4,a5,a6) ;
	fprintf(stderr,"\n");
	nerror++ ;
}

mess(str,a1,a2,a3,a4,a5,a6) /* VARARGS1 */ char *str ; {
	if ( !atend ) fprintf(stderr,"line %d: ",line) ;
	fprintf(stderr,str,a1,a2,a3,a4,a5,a6) ;
	fprintf(stderr,"\n");
}

fatal(str,a1,a2,a3,a4,a5,a6) /* VARARGS1 */ char *str ; {
	error(str,a1,a2,a3,a4,a5,a6) ;
	exit(1) ;
}

#define ILLGL   -1

check(val) int val ; {
	if ( val!=ILLGL ) error("Illegal flag combination") ;
}

int decflag(str,opc) char *str ; {
	int type ;
	int escape ;
	int range ;
	int wordm ;
	int notzero ;
	char c;

	type=escape=range=wordm=notzero= ILLGL ;
	while ( c= *str++ ) {
		switch ( c ) {
		case 'm' :
			check(type) ; type=OPMINI ; break ;
		case 's' :
			check(type) ; type=OPSHORT ; break ;
		case '-' :
			check(type) ; type=OPNO ;
			if ( (em_flag[opc]&EM_PAR)==PAR_W ) c='i' ;
			break ;
		case '1' :
			check(type) ; type=OP8 ; break ;
		case '2' :
			check(type) ; type=OP16 ; break ;
		case '4' :
			check(type) ; type=OP32 ; break ;
		case '8' :
			check(type) ; type=OP64 ; break ;
		case 'u' :
			check(type) ; type=OP16U ; break ;
		case 'e' :
			check(escape) ; escape=0 ; break ;
		case 'N' :
			check(range) ; range= 2 ; break ;
		case 'P' :
			check(range) ; range= 1 ; break ;
		case 'w' :
			check(wordm) ; wordm=0 ; break ;
		case 'o' :
			check(notzero) ; notzero=0 ; break ;
		default :
			error("Unknown flag") ;
		}
		putchar(c);
	}
	if ( type==ILLGL ) error("Type must be specified") ;
	switch ( type ) {
	case OP64 :
	case OP32 :
		if ( escape!=ILLGL ) error("Conflicting escapes") ;
		escape=ILLGL ;
	case OP16 :
	case OP16U :
	case OP8 :
	case OPSHORT :
	case OPNO :
		if ( notzero!=ILLGL ) mess("Improbable OPNZ") ;
		if ( type==OPNO && range!=ILLGL ) {
			mess("No operand in range") ;
		}
	}
	if ( escape!=ILLGL ) type|=OPESC ;
	if ( wordm!=ILLGL ) type|=OPWORD ;
	switch ( range) {
	case ILLGL : type|=OP_BOTH ; break ;
	case 1     : type|=OP_POS  ; break ;
	case 2     : type|=OP_NEG  ; break ;
	}
	if ( notzero!=ILLGL ) type|=OPNZ ;
	return type ;
}

static int pushchar ;
static int pushf ;

int readchar() {
	int c ;

	if ( pushf ) {
		pushf=0 ;
		c = pushchar ;
	} else {
		if ( feof(stdin) ) return EOF ;
		c=getc(stdin) ;
	}
	if ( c=='\n' ) line++ ;
	return c ;
}

pushback(c) {
	if ( pushf ) {
		fatal("Double pushback") ;
	}
	pushf++ ;
	pushchar=c ;
	if ( c=='\n' ) line-- ;
}
