/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 *
 */

/**********************************************************************/
/*                                                                    */
/*               Several utility routines used throughout ack         */
/*               error handling, string handling and such.            */
/*                                                                    */
/**********************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>
#include "ack.h"

#ifndef NORCSID
static char rcs_id[] = "$Id$" ;
#endif

extern  char    *progname ;
extern  int     w_flag ;
extern  int     n_error;

#ifdef DEBUG
# define STDOUT stdout
#else
# define STDOUT stderr
#endif

char *ack_basename(const char *string) {
	static char retval[256] ;
	const char *last_dot, *last_start, *fetch ;
	char *store ;
	int ctoken ;

	last_dot= (char *)0 ;
	last_start= string ;
	for ( fetch=string ; ; fetch++ ) {
		switch ( ctoken= *fetch&0377 ) {
		case SUFCHAR : last_dot=fetch ; break ;
		case '/'     : last_start=fetch+1 ; break ;
		case  0      : goto out ;
		}
		if ( !isascii(ctoken) || !isprint(ctoken) ) {
			werror("non-ascii characters in argument %s",string) ;
		}
	}
out:
	if ( ! *last_start ) fuerror("empty filename \"%s\"",string) ;
	for ( fetch= last_start, store=retval ;
		*fetch && fetch!=last_dot && store< &retval[sizeof retval-1] ;
		      fetch++, store++ ) {
				*store= *fetch ;
	}
	*store= 0 ;
	return retval ;
}

void clr_noscan(char *str) {
	register char *ptr ;
	for ( ptr=str ; *ptr ; ptr++ ) {
		*ptr&= ~NO_SCAN ;
	}
}

char *skipblank(char *str) {
	register char *ptr ;

	for ( ptr=str ; *ptr==SPACE || *ptr==TAB ; ptr++ ) ;
	return ptr ;
}

char *firstblank(char *str) {
	register char *ptr ;

	for ( ptr=str ; *ptr && *ptr!=SPACE && *ptr!=TAB ; ptr++ ) ;
	return ptr ;
}

/* VARARGS1 */
void fatal(const char* fmt, ...)
{
	/* Fatal internal error */
	va_list ap;
	va_start(ap, fmt);
	fprintf(STDOUT,"%s: fatal internal error, ",progname) ;
	vfprintf(STDOUT, fmt, ap);
	fprintf(STDOUT,"\n") ;
	quit(-2) ;
}


/* VARARGS1 */
void vprint(const char* fmt, ...)
{
	/* Diagnostic print, no auto NL */
	va_list ap;
	va_start(ap, fmt);
	vfprintf(STDOUT, fmt, ap);
	va_end(ap);
}

#ifdef DEBUG
void prns(const char *s) {
	for ( ; *s ; s++ ) {
		putc((*s&0377)&~NO_SCAN,STDOUT) ;
	}
	putc('\n',STDOUT) ;
}
#endif

/* VARARGS1 */
void fuerror(const char *fmt, ...) {
	/* Fatal user error */
	va_list ap;
	va_start(ap, fmt);
	fprintf(STDOUT,"%s: ",progname) ;
	vfprintf(STDOUT, fmt, ap);
	fprintf(STDOUT,"\n") ;
	quit(-1) ;
}

/* VARARGS1 */
void werror(const char *fmt, ...) {
	/* Warning user error, w_flag */
	va_list ap;
	if ( w_flag ) return ;
	va_start(ap, fmt);
	fprintf(STDOUT,"%s: warning, ",progname) ;
	vfprintf(STDOUT, fmt, ap);
	fprintf(STDOUT,"\n") ;
	va_end(ap);
}

/* VARARGS1 */
void error(const char *fmt, ...) {
	/* User error, it is the callers responsibility to quit */
	va_list ap;
	va_start(ap, fmt);
	fprintf(STDOUT,"%s: ",progname) ;
	vfprintf(STDOUT, fmt, ap);
	fprintf(STDOUT,"\n") ;
	n_error++ ;
	va_end(ap);
}

void quit(int code) {
	rmtemps();
	exit(code);
}

/******
	char *keeps(string)
		Keep the string in stable storage.
	throws(string)
		Remove the string stored by keep from stable storage.
		throws() is now a macro in ack.h.
***********/

char *keeps(const char *str) {
	register char *result ;
	result= getcore( (unsigned)(strlen(str)+1) ) ;
	if ( !result ) fatal("Out of core") ;
	return strcpy(result,str) ;
}

void *getcore(size_t size) {
	void *retptr ;

	retptr= calloc(1,size) ;
	if ( !retptr ) fatal("Out of memory") ;
	return retptr ;
}

void *changecore(void *ptr, size_t size) {
	void *retptr ;

	retptr= realloc(ptr,size) ;
	if ( !retptr ) fatal("Out of memory") ;
	return retptr ;
}
