/* $Id$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 *
 */

/* Author: E.G. Keizer */

/* Print a readable version of the data in the post mortem dump */
/* dmpc [-s] [-dn,m] [file] */

#include <local.h>
#include <stdio.h>
#include <ctype.h>

int dflag = 0 ;
long l_low,l_high;

int sflag;

int wsize,asize;
long tsize,dsize;
long ignmask,uerrorproc,cause;
long pc,sp,lb,hp,pd,pb;

char *cstr[] = {
	"Array bound error",
	"Range bound error",
	"Set error",
	"Integer overflow",
	"Float overflow",
	"Float underflow",
	"Divide by 0",
	"Divide by 0.0",
	"Integer undefined",
	"Float undefined",
	"Conversion error",
	"User error 11",
	"User error 12",
	"User error 13",
	"User error 14",
	"User error 15",
	"Stack overflow",
	"Heap overflow",
	"Illegal instruction",
	"Illegal size parameter",
	"Case error",
	"Memory fault",
	"Illegal pointer",
	"Illegal pc",
	"Bad argument of LAE",
	"Bad monitor call",
	"Bad line number",
	"GTO descriptor error"
};

FILE *fcore;
char *core = "core" ;
int nbyte=0;

char *pname;

int readbyte();
int read2();
long readaddr();
long readword();
unsigned getbyte();
long getword();
long getaddr();

main(argc,argv) char **argv;
{
	register i ;
	long line,fileaddr;
	char tok ;

	scanargs(argc,argv); fcore=fopen(core,"r") ;
	if ( fcore==NULL ) fatal("Can't open %s",core) ;

	if ( read2()!=010255 ) fatal("not a post mortem dump");
	if ( read2()!=VERSION ) fatal("wrong version dump file");
	wsize=read2(); asize=read2();
	if ( wsize>4 ) fatal("cannot handle word size %d",wsize) ;
	if ( asize>4 ) fatal("cannot handle pointer size %d",asize) ;
	tsize=readaddr(); dsize=readaddr();
	ignmask=readaddr(); uerrorproc=readaddr(); cause=readaddr();
	pc=readaddr(); sp=readaddr(); lb=readaddr(); hp=readaddr();
	pd=readaddr(); pb=readaddr();
	if ( sflag==0 ) {
		line=getword(0L);
		fileaddr=getaddr(4L);
		if ( fileaddr ) {
			for ( i=0 ; i<40 ; i++ ) {
				tok=getbyte(fileaddr++) ;
				if ( !isprint(tok) ) break ;
				putc(tok,stdout);
			}
			printf(" ");
		}
		if ( line ) {
			printf("line %ld",line) ;
		}
		if ( fileaddr || line ) printf(", ");
		fseek(fcore,512L,0);

		if ( cause>27 ) {
			printn("cause",cause) ;
		} else {
			prints("cause",cstr[(int)cause]);
		}
		printn("pc",pc);printn("sp",sp);printn("lb",lb);
		printn("hp",hp);
		if ( pd ) printn("pd",pd) ;
		if ( pb ) printn("pb",pb) ;
		printn("errproc",uerrorproc) ;
		printn("ignmask",ignmask) ;
		if ( tsize ) printn("Text size",tsize) ;
		if ( dsize ) printn("Data size",dsize) ;
	}
	if ( dflag==0 ) exit(0);
	fatal("d-flag not implemeted (yet)");
	exit(1) ;
}

scanargs(argc,argv) char **argv ; {
	pname=argv[0];
	while ( argv++, argc-- > 1 ) {
		switch( argv[0][0] ) {
		case '-':    switch( argv[0][1] ) {
				case 's':    sflag++ ; break ;
				case 'l':    dflag++ ; break ;
				default : fatal(": [-s] [-ln.m] [file]") ;
			} ;
			break ;
		default :core=argv[0] ;
		}
	}
}

prints(s1,s2) char *s1,*s2; {
	printf("%-15s %s\n",s1,s2);
}

printn(s1,d) char *s1; long d; {
	printf("%-15s %15ld\n",s1,d);
}

/* VARARGS1 */
fatal(s1,p1,p2,p3,p4,p5) char *s1 ; {
	fprintf(stderr,"%s: ",pname);
	fprintf(stderr,s1,p1,p2,p3,p4,p5) ;
	fprintf(stderr,"\n") ;
	exit(1) ;
}

int getb() {
	int i ;
	i=getc(fcore) ;
	if ( i==EOF ) fatal("Premature EOF");
	return i&0377 ;
}

int read2() {
	int i ;
	i=getb() ; return getb()*256 + i ;
}

long readaddr() {
	long res ;
	register int i ;

	res=0 ;
	for (i=0 ; i<asize ; i++ ) res |= getb()<<(8*i) ;
	return res ;
}

long readword() {
	long res ;
	register int i ;

	res=0 ;
	for (i=0 ; i<wsize ; i++ ) res |= getb()<<(8*i) ;
	return res ;
}

unsigned getbyte(a) long a ; {
	fseek(fcore,a+512,0) ;
	return getb() ;
}

long getword(a) long a ; {
	fseek(fcore,a+512,0) ;
	return readword() ;
}

long getaddr(a) long a ; {
	fseek(fcore,a+512,0) ;
	return readaddr() ;
}
