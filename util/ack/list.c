/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 *
 */

#include "ack.h"
#include "list.h"

#ifndef NORCSID
static char rcs_id[] = "$Id$" ;
static char rcs_list[] = RCS_LIST ;
#endif

/* List handling, operations allowed:
	adding strings to the list,
	throwing away whole lists,
	linearize a list.

Routines:
	l_add(header,string) Add an element to a list.
		header          List header, list_head *
		string          String pointer, char *
					the string is NOT copied

	l_clear(header)      Delete an whole list.
		header          List header, list_head *
	
	l_throw(header)	     Delete a list of strings.
		header          List header, list_head *

*/


l_add(header,string) list_head *header ; char *string ; {
	register list_elem *new;

	/* NOSTRICT */
	new= (list_elem *)getcore(sizeof *new);
	l_content(*new)= string ;
	/* NOSTRICT */
	l_next(*new)= (list_elem *)0 ;
	if ( !header->ca_first ) {
		header->ca_first= new ;
	} else {
		header->ca_last->ca_next= new ;
	}
	header->ca_last= new ;
}

l_clear(header) list_head *header ; {
	register list_elem *old, *next;
	for ( old=header->ca_first ; old ; old= next ) {
		next= old->ca_next ;
		freecore((char *)old) ;
	}
	header->ca_first= (list_elem *) 0 ;
	header->ca_last = (list_elem *) 0 ;
}

l_throw(header) list_head *header ; {
	register list_elem *old, *next;
	for ( old=header->ca_first ; old ; old= next ) {
		throws(l_content(*old)) ;
		next= old->ca_next ;
		freecore((char *)old) ;
	}
	header->ca_first= (list_elem *) 0 ;
	header->ca_last = (list_elem *) 0 ;
}
