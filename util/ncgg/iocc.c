/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
#ifndef NORCSID
static char rcsid[]= "$Id$";
#endif

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "param.h"
#include "set.h"
#include "expr.h"
#include "lookup.h"
#include "token.h"
#include "property.h"
#include "iocc.h"
#include <cgg_cg.h>
#include "regvar.h"
#include "extern.h"

extern set_t l_sets[];

int narexpr;
expr_t arexp[MAXATT];

expr_t iextoaddr();

iocc_t subr_iocc(tokarg,subreg) {
	inst_t insta;
	iocc_t  result;
	register i;

	insta.in_which = IN_COPY;
	insta.in_info[0] = tokarg;
	insta.in_info[1] = subreg;
	result.in_index = instalookup(insta,2);
	if (tokarg < 1) tokarg = 1;
	if (subreg==0)
		for (i=0;i<SETSIZE;i++)
			result.in_set[i] = l_sets[tokpatset[tokarg-1]].set_val[i];
	else {
		for (i=0;i<SETSIZE;i++)
			result.in_set[i] = 0;
		subregset(l_sets[tokpatset[tokarg-1]].set_val,subreg,result.in_set);
	}
	return(result);
}

iocc_t tokm_iocc(tokarg,ident) char *ident; {
	iocc_t result;
	inst_t insta;
	register i;
	char app[100];
	int dummy;
	
	for(i=0;i<SETSIZE;i++)
		result.in_set[i] = 0;
	insta.in_which = IN_MEMB;
	insta.in_info[0] = tokarg;
	if (tokarg < 1) tokarg = 1;
	sprintf(app,"%%%d.%s",tokarg,ident);
	insta.in_info[1] = 1+membset(tokpatset[tokarg-1],ident,result.in_set,
				    app,TYPREG,&dummy);
	result.in_index = instalookup(insta,2);
	return(result);
}

iocc_t percident_iocc(ident) char *ident; {
	iocc_t result;
	inst_t insta;
	register i;
	char app[100];
	int dummy;
	
	for(i=0;i<SETSIZE;i++)
		result.in_set[i] = 0;
	insta.in_which = IN_MEMB;
	insta.in_info[0] = 0;
	sprintf(app,"%%%s",ident);
	insta.in_info[1] = 1+membset(cursetno,ident,result.in_set,
				    app,TYPREG,&dummy);
	result.in_index = instalookup(insta,2);
	return(result);
}

iocc_t ident_iocc(ident) char *ident; {
	iocc_t result;
	inst_t insta;
	register i;
	register symbol *sy_p;

	for(i=0;i<SETSIZE;i++)
		result.in_set[i] = 0;
	insta.in_which = IN_RIDENT;
	sy_p = lookup(ident,symreg,mustexist);
	insta.in_info[0] = sy_p->sy_value.syv_regno;
	result.in_index = instalookup(insta,1);
	BIS(result.in_set,sy_p->sy_value.syv_regno);
	return(result);
}

iocc_t all_iocc(all_no,subreg) {
	iocc_t result;
	inst_t insta;
	register i;
	set_t localset;
	register short *sp;

	sp = l_props[allreg[all_no]].pr_regset;
	for (i=0;i<SETSIZE;i++)
		localset.set_val[i] = i<SZOFSET(MAXREGS) ? sp[i] : 0;
	for(i=0;i<SETSIZE;i++)
		result.in_set[i] = 0;
	insta.in_which = IN_ALLOC;
	insta.in_info[0] = all_no;
	insta.in_info[1] = subreg;
	subregset(localset.set_val,subreg,result.in_set);
	result.in_index = instalookup(insta,2);
	return(result);
}

iocc_t descr_iocc(ident) char *ident; {
	iocc_t result;
	inst_t insta;
	register symbol *sy_p;
	register token_p tp;
	register i;
	int typerr;

	for(i=0;i<SETSIZE;i++)
		result.in_set[i] = 0;
	sy_p = lookup(ident,symtok,mustexist);
	tp = l_tokens[sy_p->sy_value.syv_tokno];
	BIS(result.in_set,sy_p->sy_value.syv_tokno+nregs);
	insta.in_which = IN_DESCR;
	if (rvused&SL_REGVAR && strcmp(ident,"LOCAL")==0)
		insta.in_which = IN_S_DESCR;
	else if (rvused&DL_REGVAR && strcmp(ident,"DLOCAL")==0)
		insta.in_which = IN_D_DESCR;
	insta.in_info[0] = sy_p->sy_value.syv_tokno;
	for (i=0;i<MAXATT;i++) {
		if (tp->tk_att[i].ta_type == -3) {
			if (narexpr>i)
				error("token %s initialized with too many attributes",ident);
			break;
		}
		if (i>= narexpr) {
			error("token %s initialized with too few attributes",
					ident);
			break;
		}
		typerr = 0;
		switch(arexp[i].ex_typ) {
		default: assert(0);
		case TYPINT:
			if (tp->tk_att[i].ta_type != -1)
				if (tp->tk_att[i].ta_type == -2)
					arexp[i] = iextoaddr(arexp[i]);
				else
					typerr++;
			break;
		case TYPBOOL:
			typerr++; break;
		case TYPADDR:
			if (tp->tk_att[i].ta_type != -2)
				typerr++;
			break;
		case TYPREG:
			if (tp->tk_att[i].ta_type<0)
				typerr++;
			else if (!subset(arexp[i].ex_regset,
			           l_props[tp->tk_att[i].ta_type].pr_regset,
				   SZOFSET(MAXREGS)))
				typerr++;
			break;
		}
		if (typerr)
			error("Attribute %s.%s given wrong type of value",
				ident,tp->tk_att[i].ta_name);
		insta.in_info[i+1] = arexp[i].ex_index;
	}
	result.in_index = instalookup(insta,i+1);
	return(result);
}

/* low level instance package */

int ninstances=1;
inst_t l_instances[MAXINSTANCES];

instalookup(insta,filled) inst_t insta; {
	register i,j;

	for (j=filled;j<=MAXATT;j++)
		insta.in_info[j] = 0;
	for (i=0;i<ninstances;i++) {
		if (insta.in_which != l_instances[i].in_which)
			continue;
		for(j=0;j<=MAXATT;j++)
			if (insta.in_info[j]!= l_instances[i].in_info[j])
				goto cont;
		return(i);
	    cont:;
	}
	NEXT(ninstances,MAXINSTANCES,"Instances");
	l_instances[i] = insta;
	return(i);
}
