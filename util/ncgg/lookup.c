/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
#ifndef NORCSID
static char rcsid[]= "$Id$";
#endif

#include "assert.h"
#include "param.h"
#include "lookup.h"

char *myalloc();
char *mystrcpy();

symbol dumsym;	/* dummy to return in case of error */

symbol *lookup(name,type,style)
char *name;
symtype type;
lookupstyle style;
{
	symbol *sy_p,**sy_pp;

	for (sy_pp = &symhash[hashvalue(name)];(sy_p= *sy_pp) != 0;sy_pp= &sy_p->sy_next) {
		if (strcmp(sy_p->sy_name,name)!=0)
			continue;
		switch(style) {
		default:
			assert(0);
		case justlooking:
		case mustexist:
		case makeexist:
			if (type==symany || type==sy_p->sy_type)
				return(sy_p);
			continue;
		case newsymbol:
			error("%s already defined",name);
			return(&dumsym);
		}
	}
	switch(style) {
	default:
		assert(0);
	case justlooking:
		return((symbol *) 0);
	case mustexist:
		fatal("%s is unknown symbol",name);
		/* NOTREACHED */
	case newsymbol:
	case makeexist:
		NEW(sy_p,symbol);
		sy_p->sy_next = 0;
		sy_p->sy_name = mystrcpy(name);
		assert(type!=symany);
		sy_p->sy_type = type;
		*sy_pp = sy_p;
		return(sy_p);
	}
}

hashvalue(s) register char *s; {
	register unsigned sum=0;
	register i;

	for(i=0;*s;s++,i=(i+3)&07)
		sum += *s<<i;
	return(sum%NSYMHASH);
}
