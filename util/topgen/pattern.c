/* $Id$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* p a t t e r n . c
 *
 * Deals with the pattern stuff.
 * it maintains a table of information about the patterns
 * Functions : addpattern() and printpatterns()
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "misc.h"
#include "symtab.h"

struct pattern {
    char *p_constraint;			/* constraint of this pattern */
    int p_lineno, 			/* line number of constraint */
        p_npat, 			/* # of instructions in pattern */
        p_nrepl;			/* # of instructions in replacement */
};

static struct pattern *pattable,	/* ptr to pattern array */
		      *current,		/* ptr to first unoccupied el of
					 * pattern array
					 */
		      *maxpat;		/* if beyond this, new space must
					 * be allocated
					 */

void addpattern(char* str,int l,int np,int nr)
{
    /*
     * Just add a pattern to the list.
     * "str" is the constraint, "l" is the line number,
     * "np" is the number of instructions in the pattern,
     * "nr" is the number of instructions in the replacement
     * Space is allocated in chunks of 50
     */
    register struct pattern *p;

    if (!pattable) {		/* No space allocated yet */
	pattable = (struct pattern *) malloc(50 * sizeof *pattable);
	current = pattable;
	maxpat = pattable + 50;
    }
    if (current >= maxpat) {	/* Allocate some new space */
	p = pattable;
	pattable = (struct pattern *) realloc(
		(char *) pattable,
		(unsigned) (sizeof *pattable * (50 + (maxpat - pattable))));
	current = pattable + (current - p);
	maxpat = pattable + (maxpat - p) + 50;
    }
    p = current++;
    p->p_constraint = str;
    p->p_lineno = l;
    p->p_npat = np;
    p->p_nrepl = nr;
}

static void prconstraint(char* str)
{
    /*
     * prints a constraint, with variable names replaced
     */
    char c;
    register char *p, *q;
    struct symtab *name;

    p = str;
    while (*p) {
	if (isupper(*p) || islower(*p) || *p == '_') {
	    /*
	     * Start of identifier
	     */
	    q = p + 1;
	    while (*q && (
		   isupper(*q) || islower(*q) || isdigit(*q) || *q == '_')) {
		q++;
	    }
	    c = *q;
	    *q = '\0';
	    /* Temporarily let it end with null byte */
	    name = findident(p,LOOKING,&idtable);
	    if (name) {		/* yeah, it was a variable */
		fprintf(genc,"var[%d].value", name->s_num);
	    }
	    else if (!strcmp(p, "ANY")) {
		fputs("ANY.value", genc);
	    }
	    else fputs(p,genc);
	    /* Now replace null byte with whatever used to be there */
	    *q = c;
	    p = q;
	}
	else {
	    putc(*p,genc);
	    p++;
	}
    }
}

void printpatterns(void) {
    /*
     * Prints the pattern_descr table and generates the routine
     * "check_constraint"
     */
    register struct pattern *p;
    register int i;

    p = pattable;
    i = 1;
    fputs("struct pattern_descr patterns[] = {\n", genc);
    while (p != current) {
	fprintf(genc," {%d,pat%d,%d,rep%d,},\n",
			p->p_npat, i, p->p_nrepl, i);
	p++;
	i++;
    }
    fputs("};\n", genc);
    fputs("int\ncheck_constraint(patno){\n\tint r;\n\tswitch(patno){\n",genc);
    p = pattable;
    while (p < current) {
	if (p->p_constraint) {
	    /* The pattern has a constraint */	
	    fprintf(genc,"\tcase %d :\n",(int)(p - pattable));
	    fprintf(genc,linedir,p->p_lineno,inpfile);	/* linedirective */
	    fputs("\tr = (",genc);
	    prconstraint(p->p_constraint);
	    fputs("); break;\n",genc);
	}
	p++;
    }
    fputs("\tdefault :\n\t\tr = 1;\n\t}\n\treturn r;\n}\n\n",genc);
}
