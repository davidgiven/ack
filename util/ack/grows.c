/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 *
 */

/**************************************************************************/
/*                                                                        */
/*                 Bookkeeping for growing strings                        */
/*                                                                        */
/**************************************************************************/

#include "ack.h"
#include "grows.h"

#ifndef NORCSID
static char rcs_id[] = "$Id$" ;
static char rcs_grows[] = RCS_GROWS ;
#endif

gr_add(id,c) register growstring *id ; char c ; {
	if ( id->gr_size==id->gr_max) {
		if ( id->gr_size==0 ) { /* The first time */
			id->gr_max= 2*GR_MORE ;
			id->gr_string= getcore(id->gr_max) ;
		} else {
			id->gr_max += GR_MORE ;
			id->gr_string= changecore(id->gr_string,id->gr_max ) ;
		}
	}
	*(id->gr_string+id->gr_size++)= c ;
}

gr_cat(id,string) growstring *id ; char *string ; {
	register char *ptr ;

#ifdef DEBUG
	if ( id->gr_size && *(id->gr_string+id->gr_size-1) ) {
		vprint("Non-zero terminated %*s\n",
			id->gr_size, id->gr_string ) ;
	}
#endif
	if ( id->gr_size ) id->gr_size-- ;
	ptr=string ;
	for (;;) {
		gr_add(id,*ptr) ;
		if ( *ptr++ ) continue ;
		break ;
	}
}

gr_throw(id) register growstring *id ; {
	/* Throw the string away */
	if ( id->gr_max==0 ) return ;
	freecore(id->gr_string) ;
	id->gr_string = 0 ;
	id->gr_max=0 ;
	id->gr_size=0 ;
}

gr_init(id) growstring *id ; {
	id->gr_size=0 ; id->gr_max=0 ;
}

char *gr_final(id) growstring *id ; {
	/* Throw away the bookkeeping, adjust the string to its final
	   length and return a pointer to a string to be get rid of with
	   throws
	*/
	register char *retval ;
	retval= keeps(gr_start(*id)) ;
	gr_throw(id) ;
	return retval ;
}
