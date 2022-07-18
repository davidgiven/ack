/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 *
 */

#include "ass00.h"
#include "assex.h"
#include "assrl.h"
#include <stddef.h>
#include <stdio.h>
#include <stdarg.h>
#include "system.h"


/*
 * this file contains several library routines.
 */

static char* filename;


/* VARARGS1 */
static void pr_error(const char* string1, va_list ap) {
	/*
	 * diagnostic output
	 */
	fprintf(stderr,"%s: ",progname);
	if (curfile) {
		fprintf(stderr,"file %s",curfile);
		if (archmode)
			fprintf(stderr," (%.14s)",archhdr.ar_name);
		fprintf(stderr,": ");
	}
	if ( pstate.s_curpro ) {
		fprintf(stderr,"proc %s, ",pstate.s_curpro->p_name);
	}
	fprintf(stderr,"line %d: ",line_num);
	vfprintf(stderr,string1,ap);
	fprintf(stderr,"\n");
}

/* VARARGS1 */
void error(const char* string1, ...)
{
	va_list ap;
	va_start(ap, string1);
	pr_error(string1, ap);
	va_end(ap);
	nerrors++ ;
}

/* VARARGS1 */
void werror(const char* string1, ...)
{
	va_list ap;
	if ( wflag ) return ;

	va_start(ap, string1);
	pr_error(string1, ap);
	va_end(ap);
}

void fatal(char *s)
{
	/*
	 * handle fatal errors
	 */
	error("Fatal error: %s",s);
	dump(0);
	exit(EXIT_FAILURE);
}



int xgetc(register FILE *af)
{
	register int nextc;
	nextc=fgetc(af) ;
	if ( feof(af) )
			fatal("unexpected end of file");
	return nextc ;
}

void xputc(int c,register FILE *af)
{
	fputc(c,af) ;
	if ( ferror(af) ) fatal("write error") ;
}


void putblk(register FILE *stream,register char *from, register int amount)
{
	for ( ; amount-- ; from++ ) {
		fputc(*from,stream) ;
		if ( ferror(stream) ) fatal("write error") ;
	}
}

int getblk(register FILE *stream, register char *from, register int amount)
{
	for ( ; amount-- ; from++ ) {
		*from = fgetc(stream) ;
		if ( feof(stream) ) return 1 ;
	}
	return 0 ;
}

void xput16(int w,FILE *f)
{
	/*
	 * two times xputc
	 */
	xputc(w,f);
	xputc(w>>8,f);
}

void xputarb(int l,cons_t w, FILE* f)
{
	while ( l-- ) {
		xputc( int_cast w,f) ;
		w >>=8 ;
	}
}

void put8(int n)
{
	xputc(n,tfile);
	textoff++;
}

void put16(int n)
{
	/*
	 * note reversed order of bytes.
	 * this is done for faster interpretation.
	 */
	xputc(n>>8,tfile);
	xputc(n&0377,tfile);
	textoff += 2;
}

void put32(cons_t n)
{
	put16( int_cast (n>>16)) ;
	put16( int_cast n) ;
}

void put64(cons_t n)
{
	fatal("put64 called") ;
}

int xget8(void)
{
	/*
	 * Read one byte from ifile.
	 */
	if (libeof && inpoff >= libeof)
		return EOF ;
	inpoff++;
	return fgetc(ifile) ;
}

unsigned int get8(void)
{
	register int nextc;
	/*
	 * Read one byte from ifile.
	 */
	nextc=xget8();
	if ( nextc==EOF ) {
		if (libeof)
			fatal("Tried to read past end of arentry\n");
		else
			fatal("end of file on input");
	}
	return nextc ;
}

cons_t xgetarb(int l,FILE *f)
{
	cons_t val ;
	register int shift ;
	int c;

	shift=0 ; val=0 ;
	while ( l-- ) {
		/* val += ((cons_t)(c = ctrunc(xgetc(f))))<<shift ;
		   Bug here: shifts with too large shift counts
		  get unspecified results. --Ceriel */
		c = ctrunc(xgetc(f));
		if (shift < 8 * sizeof(cons_t)) {
			val += ((cons_t)c)<<shift ;
		}
		shift += 8 ;
	}
	if (c == 0377 && shift > 8 && ((shift>>3)&1)) {
		while (shift < 8*sizeof(cons_t)) {
			val += ((cons_t)c)<<shift ;
			shift += 8;
		}
	}
	return val ;
}

void ext8(int b)
{
	/*
	 * Handle one byte of data.
	 */
	++dataoff;
	xputc(b,dfile);
}

void extword(cons_t w)
{
	/* Assemble the word constant w.
	 * NOTE: The bytes are written low to high.
	 */
	register int i ;
	for ( i=wordsize ; i-- ; ) {
		ext8( int_cast w) ;
		w >>= 8 ;
	}
}

void extarb(int size, long value)
{
	/* Assemble the 'size' constant value.
	 * The bytes are again written low to high.
	 */
	register int i ;
	for ( i=size ; i-- ; ) {
		ext8( int_cast value ) ;
		value >>=8 ;
	}
}

void extadr(cons_t a)
{
	/* Assemble the pointer constant a.
	 * NOTE: The bytes are written low to high.
	 */
	register int i ;
	for ( i=ptrsize ; i-- ; ) {
		ext8( int_cast a) ;
		a >>= 8 ;
	}
}

void xputa(cons_t a,FILE* f)
{

	register int i ;
	for ( i=ptrsize ; i-- ; ) {
		xputc( int_cast a,f) ;
		a >>= 8 ;
	}
}

cons_t xgeta(FILE* f)
{

	register int i, shift ;
	cons_t val ;
	val = 0 ; shift=0 ;
	for ( i=ptrsize ; i-- ; ) {
		val += ((cons_t)xgetc(f))<<shift ;
		shift += 8 ;
	}
	return val ;
}

int icount(int size)
{
	int amount ;
	amount=(dataoff-lastoff)/size ;
	if ( amount>MAXBYTE) fatal("Descriptor overflow");
	return amount ;
}

void set_mode(int mode)
{

	if (datamode==mode) {   /* in right mode already */
		switch ( datamode ) {
		case DATA_CONST:
			if ( (dataoff-lastoff)/wordsize < MAXBYTE ) return ;
			break ;
		case DATA_BYTES:
			if ( dataoff-lastoff < MAXBYTE ) return ;
			break ;
		case DATA_IPTR:
		case DATA_DPTR:
			if ( (dataoff-lastoff)/ptrsize < MAXBYTE ) return ;
			break ;
		case DATA_ICON:
		case DATA_FCON:
		case DATA_UCON:
		case DATA_BSS:
			break ;
		default:
			return ;
		}
		set_mode(DATA_NUL) ; /* flush current descriptor */
		set_mode(mode) ;
		return;
	}
	switch(datamode) {              /* terminate current mode */
	case DATA_NUL:
		break;                  /* nothing to terminate */
	case DATA_CONST:
		lastheader->r_val.rel_i=icount(wordsize) ;
		lastheader->r_typ = RELHEAD;
		datablocks++;
		break;
	case DATA_BYTES:
		lastheader->r_val.rel_i=icount(1) ;
		lastheader->r_typ = RELHEAD;
		datablocks++;
		break;
	case DATA_DPTR:
	case DATA_IPTR:
		lastheader->r_val.rel_i=icount(ptrsize) ;
		lastheader->r_typ = RELHEAD;
		datablocks++;
		break;
	default:
		datablocks++;
		break;
	}
	datamode=mode;
	switch(datamode) {
	case DATA_NUL:
		break;
	case DATA_CONST:
		ext8(HEADCONST);
		lastheader=data_reloc( chp_cast 0,dataoff,RELNULL);
		ext8(0);
		lastoff=dataoff;
		break;
	case DATA_BYTES:
		ext8(HEADBYTE);
		lastheader=data_reloc( chp_cast 0,dataoff,RELNULL);
		ext8(0);
		lastoff=dataoff;
		break;
	case DATA_IPTR:
		ext8(HEADIPTR);
		lastheader=data_reloc( chp_cast 0,dataoff,RELNULL);
		ext8(0);
		lastoff=dataoff;
		break;
	case DATA_DPTR:
		ext8(HEADDPTR);
		lastheader=data_reloc( chp_cast 0,dataoff,RELNULL);
		ext8(0);
		lastoff=dataoff;
		break;
	case DATA_ICON:
		ext8(HEADICON) ;
		ext8( int_cast consiz) ;
		break;
	case DATA_FCON:
		ext8(HEADFCON) ;
		ext8( int_cast consiz) ;
		break;
	case DATA_UCON:
		ext8(HEADUCON) ;
		ext8( int_cast consiz) ;
		break;
	case DATA_REP:
		ext8(HEADREP) ;
		break ;
	case DATA_BSS:
		ext8(HEADBSS) ;
		break;
	default:
		fatal("Unknown mode in set_mode") ;
	}
}


char* tmpfil(void)
{
	return sys_maketempfile("ack-ass", "dat");
}
