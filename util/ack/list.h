/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
#ifndef NORCSID
#define RCS_LIST "$Id$"
#endif

struct ca_elem {
	struct ca_elem          *ca_next; /* The link */
	char                    *ca_cont; /* The contents */
} ;

struct ca_list {
	struct ca_elem          *ca_first; /* The head */
	struct ca_elem          *ca_last;  /* The tail */
} ;

typedef struct ca_list list_head ;         /* The decl. for headers */
typedef struct ca_elem list_elem ;         /* The decl. for elements */

/* Some operations */

/* Access */
#define l_first(header)         (header).ca_first
#define l_next(elem)            (elem).ca_next
#define l_content(elem)         (elem).ca_cont

/* To be used for scanning lists, ptr is the running variable */
#define scanlist(elem,ptr) \
	for ( ptr= elem ; ptr; ptr= l_next(*ptr) )
