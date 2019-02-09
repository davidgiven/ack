/* $Id$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* s y m t a b . c 
 *
 * Contains the routine findident, which builds the symbol table and
 * searches identifiers
 */
 
#include <stdlib.h>
#include <string.h>
#include "symtab.h"

struct symtab *idtable, *deftable;

extern void error(char *s, char* s1);

struct symtab *
findident(s, mode, table) char *s; struct symtab **table; {
    /*
     * Look for identifier s in the symboltable referred to by *table.
     * If mode = LOOKING, no new entry's will be made.
     * If mode = ENTERING, a new entry will be made if s is not in the
     * table yet, otherwise an error results
     */
    register struct symtab *p;
    register int n;

    if (!*table) {	/* No entry for this symbol */
	if (mode == LOOKING) return (struct symtab *) 0;
	/*
	 * Make new entry
	 */
	p = (struct symtab *) malloc(sizeof *p);
	p->s_left = p->s_right = (struct symtab *) 0;
	p->s_name = malloc( (unsigned) (strlen(s) + 1));
	strcpy(p->s_name,s);
	*table = p;
	return p;
    }
    else {
	p = *table;
	if ((n = strcmp(p->s_name,s)) == 0) {	/* This is it! */
	    if (mode == ENTERING) {
		error("Identifier %s redefined",s);
	    }
	    return p;
	}
	/* Binary tree ..... */
	if (n < 0) return findident(s,mode,&(p->s_left));
	return findident(s,mode,&(p->s_right));
    }				
    /* NOTREACHED */
}
