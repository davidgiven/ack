#ifndef NORCSID
static char rcsid[]= "$Header$";
#endif

#include "param.h"
#include "property.h"
#include "set.h"
#include "token.h"
#include "lookup.h"
#include <cgg_cg.h>
#include "extern.h"

extern set_t l_sets[];

setlookup(s) set_t s; {
	register set_p p;
	register i;
	int setno;

	for(p=l_sets;p<&l_sets[nsets];p++) {
		if (p->set_size != s.set_size)
			continue;
		for (i=0;i<SETSIZE;i++)
			if (p->set_val[i] != s.set_val[i])
				goto cont;
		return(p-l_sets);
	    cont:;
	}
	setno = NEXT(nsets,MAXSETS,"Sets");
	l_sets[setno] = s;
	return(setno);
}

make_std_sets() {
	set_t s;
	register i;

	for(i=0;i<SETSIZE;i++)
		s.set_val[i]=0;
	for(i=0;i<nregs+ntokens;i++)
		BIS(s.set_val,i);
	s.set_size = 0;
	allsetno = setlookup(s);
	n_set("ALL",allsetno);
}

set_t emptyset;

set_t ident_to_set(name) char *name; {
	register symbol *sy_p;
	register i;
	register struct propinfo *pp;
	int bitno;
	set_t result;

	sy_p = lookup(name,symany,mustexist);
	switch(sy_p->sy_type) {
	default:
		error("%s is wrong kind of symbol",name);
		return(emptyset);
	case symprop:
		pp = &l_props[sy_p->sy_value.syv_propno];
		result.set_size = pp->pr_size;
		for (i=0;i<SZOFSET(MAXREGS);i++)
			result.set_val[i] = pp->pr_regset[i];
		BIS(result.set_val,0);
		for (;i<SETSIZE;i++)
			result.set_val[i] = 0;
		break;
	case symtok:
		bitno = sy_p->sy_value.syv_tokno+nregs;
		for (i=0;i<SETSIZE;i++)
			result.set_val[i] = 0;
		BIS(result.set_val,bitno);
		result.set_size = l_tokens[sy_p->sy_value.syv_tokno]->tk_size;
		break;
	case symset:
		return(l_sets[sy_p->sy_value.syv_setno]);
	}
	return(result);
}

set_t setproduct(s1,s2) set_t s1,s2; {
	set_t result;
	register i;

	if ((result.set_size=s1.set_size)==0)
		result.set_size = s2.set_size;
	for(i=0;i<SETSIZE;i++)
		result.set_val[i] = s1.set_val[i] & s2.set_val[i];
	return(result);
}

set_t setsum(s1,s2) set_t s1,s2; {
	set_t result;
	register i;

	if (s1.set_size == s2.set_size)
		result.set_size = s1.set_size;
	else
		result.set_size = 0;
	for(i=0;i<SETSIZE;i++)
		result.set_val[i] = s1.set_val[i] | s2.set_val[i];
	return(result);
}

set_t setdiff(s1,s2) set_t s1,s2; {
	set_t result;
	register i;

	if (s1.set_size == s2.set_size)
		result.set_size = s1.set_size;
	else
		result.set_size = 0;
	for(i=0;i<SETSIZE;i++)
		result.set_val[i] = s1.set_val[i] & ~ s2.set_val[i];
	return(result);
}
