/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
#ifndef NORCSID
static char rcsid[]= "$Id$";
#endif

#include "param.h"
#include "instruct.h"
#include "pseudo.h"
#include "varinfo.h"
#include "set.h"
#include "expr.h"
#include "iocc.h"
#include <cgg_cg.h>
#include "extern.h"

extern int niops;
extern iocc_t iops[];
extern inproc;

extern set_t l_sets[];
extern inst_t l_instances[];

extern expr_t subreg_expr(),regno_expr();

struct varinfo * setcoco(n) {
	struct varinfo *vi;
	
	NEW(vi,struct varinfo);
	vi->vi_next = VI_NULL;
	vi->vi_int[0] = INSSETCC;
	vi->vi_int[1] = n;
	return(vi);
}

struct varinfo * generase(n) {
	struct varinfo *vi;

	NEW(vi,struct varinfo);
	vi->vi_next = VI_NULL;
	vi->vi_int[0] = INSERASE;
	vi->vi_int[1] = n;
	return(vi);
}

struct varinfo * genremove(n) {
	struct varinfo *vi;

	NEW(vi,struct varinfo);
	vi->vi_next = VI_NULL;
	vi->vi_int[0] = INSREMOVE;
	vi->vi_int[1] = n;
	return(vi);
}

onlyreg(argno) {
	register bitno;
	register short *sp;
	
	if (! argno) argno++;
	sp = l_sets[tokpatset[argno-1]].set_val;
	for(bitno=nregs;bitno<nregs+ntokens;bitno++)
		if (BIT(sp,bitno))
			return(0);
	return(1);
}

makescratch(argno) {
	set_t s;

	if (! argno) argno++;
	if (tokpatro[argno-1])
		error("Instruction destroys %%%d, not allowed here",argno);
	s = l_sets[tokpatset[argno-1]];
	BIC(s.set_val,0);
	tokpatset[argno-1] = setlookup(s);
}

struct varinfo *gen_inst(ident,star) char *ident; {
	register struct varinfo *vi,*retval,*eravi;
	register instr_p ip;
	register struct operand *op;
	register i;
	register inst_p insta;
	
	if (star && !inproc)
		error("Variable instruction only allowed inside proc");
	for (ip=l_instr;ip<l_instr+ninstr;ip++) {
		if(strcmp(ident,ip->i_name))
			continue;
		if (ip->i_nops!=niops)
			continue;
		for(i=0,op=ip->i_oplist;i<niops;i++,op=op->o_next) {
			if (!subset(iops[i].in_set,l_sets[op->o_setno].set_val,SETSIZE))
				goto cont;
		}
		goto found;		/* oh well, one more won't hurt */
	    cont:;
	}
	error("Such an \"%s\" does not exist",ident);
	return(0);
found:
	NEW(vi,struct varinfo);
	vi->vi_int[0] = ip-l_instr;
	vi->vi_int[1] = star;
	vi->vi_next=0;
	retval = vi;
	for(i=0;i<niops;i++) {
		NEW(vi->vi_vi,struct varinfo);
		vi=vi->vi_vi;
		vi->vi_int[0] = iops[i].in_index;
	}
	vi->vi_vi = 0;
	vi = retval;
	for(i=0,op=ip->i_oplist;i<niops;i++,op=op->o_next) {
	    if(op->o_adorn&AD_CC) {
		vi->vi_next = setcoco(iops[i].in_index);
		vi=vi->vi_next;
	    }
	    switch(op->o_adorn&AD_RWMASK) {
	    default:
		/* Nothing possible to do */
		break;
	    case AD_RO:
		/* It might be possible to do something
		 * here but not now.
		 */
		break;
	    case AD_RW:
	    case AD_WO:
		/* Treated the same for now */
		insta = &l_instances[iops[i].in_index];
		switch(insta->in_which) {
		case IN_COPY:
			if(insta->in_info[1]==0 && !onlyreg(insta->in_info[0]))
				break;
			makescratch(insta->in_info[0]);
			vi->vi_next = generase(
			               ex_lookup(
				        EX_SUBREG,insta->in_info[0],
					          insta->in_info[1]
				       )
				      );
			vi = vi->vi_next;
			break;
		case IN_MEMB:
			vi->vi_next = generase(
				       ex_lookup(
				        EX_TOKFIELD,insta->in_info[0],
					            insta->in_info[1]
				       )
				      );
			vi=vi->vi_next;
			break;
		case IN_RIDENT:
			vi->vi_next = generase(
				       ex_lookup(
				        EX_REG,insta->in_info[0],0
				       )
				      );
			vi = vi->vi_next;
			break;
		case IN_ALLOC:
			vi->vi_next = generase(
				       ex_lookup(
				        EX_ALLREG,insta->in_info[0]+1,
						  insta->in_info[1]
				       )
				      );
			vi = vi->vi_next;
			break;
		case IN_S_DESCR:
		case IN_D_DESCR:
			{ int temp;

			temp=ex_lookup(EX_REGVAR,insta->in_info[1],0);
			vi->vi_next = generase(temp);
			vi = vi->vi_next;
			vi->vi_next = genremove(temp);
			vi = vi->vi_next;
			break;
			}
		}
		break;
	    }
	}
	for (eravi=ip->i_erases;eravi != VI_NULL;eravi=eravi->vi_next) {
		if (eravi->vi_int[0] < 0)
			vi->vi_next = setcoco(0);
		else {
			vi->vi_next = generase(eravi->vi_int[0]);
			vi=vi->vi_next;
			vi->vi_next = genremove(eravi->vi_int[0]);
		}
		vi=vi->vi_next;
	}
	return(retval);
}
