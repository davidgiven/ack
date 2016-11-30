/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
#ifndef NORCSID
#define RCS_GROWS "$Id$"
#endif

/* struct used to identify and do bookkeeping for growing strings */

typedef struct {
	char            *gr_string ;    /* Points to start of string */
	unsigned        gr_size ;       /* Current string size */
	unsigned        gr_max ;        /* Maximum string size */
} growstring ;

#define GR_MORE         50      /* Steps to grow */

#define gr_start(id)    (id).gr_string  /* The start of the string */

/* Routines used */

void     gr_throw(growstring *) ;               /* To free the core */
int      gr_add(growstring *, int) ;            /* To add one character */
int      gr_cat(growstring *, const char *) ;   /* To append a string */
void     gr_init(growstring *) ;        /* Initialize the bookkeeping */
char    *gr_final(growstring *) ;       /* Move to a stable storage string */
