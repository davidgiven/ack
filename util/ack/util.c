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

char *basename(string) char *string ; {
	static char retval[256] ;
	char *last_dot, *last_start ;
	register char *store;
	register char *fetch ;
	register int ctoken ;

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

clr_noscan(str) char *str ; {
	register char *ptr ;
	for ( ptr=str ; *ptr ; ptr++ ) {
		*ptr&= ~NO_SCAN ;
	}
}

char *skipblank(str) char *str ; {
	register char *ptr ;

	for ( ptr=str ; *ptr==SPACE || *ptr==TAB ; ptr++ ) ;
	return ptr ;
}

char *firstblank(str) char *str ; {
	register char *ptr ;

	for ( ptr=str ; *ptr && *ptr!=SPACE && *ptr!=TAB ; ptr++ ) ;
	return ptr ;
}

/* VARARGS1 */
fatal(fmt,p1,p2,p3,p4,p5,p6,p7) char *fmt ; {
	/* Fatal internal error */
	fprintf(STDOUT,"%s: fatal internal error, ",progname) ;
	fprintf(STDOUT,fmt,p1,p2,p3,p4,p5,p6,p7);
	fprintf(STDOUT,"\n") ;
	quit(-2) ;
}


/* VARARGS1 */
vprint(fmt,p1,p2,p3,p4,p5,p6,p7) char *fmt ; {
	/* Diagnostic print, no auto NL */
	fprintf(STDOUT,fmt,p1,p2,p3,p4,p5,p6,p7);
}

#ifdef DEBUG
prns(s) register char *s ; {
	for ( ; *s ; s++ ) {
		putc((*s&0377)&~NO_SCAN,STDOUT) ;
	}
	putc('\n',STDOUT) ;
}
#endif

/* VARARGS1 */
fuerror(fmt,p1,p2,p3,p4,p5,p6,p7) char *fmt ; {
	/* Fatal user error */
	fprintf(STDOUT,"%s: ",progname) ;
	fprintf(STDOUT,fmt,p1,p2,p3,p4,p5,p6,p7);
	fprintf(STDOUT,"\n") ;
	quit(-1) ;
}

/* VARARGS1 */
werror(fmt,p1,p2,p3,p4,p5,p6,p7) char *fmt ; {
	/* Warning user error, w_flag */
	if ( w_flag ) return ;
	fprintf(STDOUT,"%s: warning, ",progname) ;
	fprintf(STDOUT,fmt,p1,p2,p3,p4,p5,p6,p7);
	fprintf(STDOUT,"\n") ;
}

/* VARARGS1 */
error(fmt,p1,p2,p3,p4,p5,p6,p7) char *fmt ; {
	/* User error, it is the callers responsibility to quit */
	fprintf(STDOUT,"%s: ",progname) ;
	fprintf(STDOUT,fmt,p1,p2,p3,p4,p5,p6,p7);
	fprintf(STDOUT,"\n") ;
	n_error++ ;
}

do_flush() {
	fflush(stdout) ;
	fflush(stderr) ;
}

void
noodstop() {
	quit(-3) ;
}

quit(code) {
	rmtemps();
	exit(code);
}
/******
	char *keeps(string)
		Keep the string in stable storage.
	throws(string)
		Remove the string stored by keep from stable storage.
***********/

char *keeps(str) char *str ; {
	register char *result ;
	result= getcore( (unsigned)(strlen(str)+1) ) ;
	if ( !result ) fatal("Out of core") ;
	return strcpy(result,str) ;
}

throws(str) char *str ; {
	freecore(str) ;
}

char *getcore(size) unsigned size ; {
	register char *retptr ;

	retptr= calloc(1,size) ;
	if ( !retptr ) fatal("Out of memory") ;
	return retptr ;
}

char *changecore(ptr,size) char *ptr ; unsigned size ; {
	register char *retptr ;

	retptr= realloc(ptr,size) ;
	if ( !retptr ) fatal("Out of memory") ;
	return retptr ;
}
