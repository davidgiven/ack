/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 *
 * Author: Hans van Staveren
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "assert.h"
#include "param.h"
#include "tables.h"
#include "types.h"
#include <cg_pattern.h>
#include "data.h"
#include "result.h"
#include "glosym.h"
#include "extern.h"
#include "subr.h"
#include "regvar.h"
#include "salloc.h"


#define LLEAF 01
#define LDEF  02
#define RLEAF 04
#define RDEF  010
#define LLDEF LLEAF|LDEF
#define RLDEF RLEAF|RDEF

char opdesc[] = {
	0,                      /* EX_TOKFIELD */
	0,                      /* EX_ARG */
	0,                      /* EX_CON */
	0,                      /* EX_ALLREG */
	LLDEF|RLDEF,            /* EX_SAMESIGN */
	LLDEF|RLDEF,            /* EX_SFIT */
	LLDEF|RLDEF,            /* EX_UFIT */
	0,                      /* EX_ROM */
	LLDEF|RLDEF,            /* EX_NCPEQ */
	LLDEF|RLDEF,            /* EX_SCPEQ */
	LLDEF|RLDEF,            /* EX_RCPEQ */
	LLDEF|RLDEF,            /* EX_NCPNE */
	LLDEF|RLDEF,            /* EX_SCPNE */
	LLDEF|RLDEF,            /* EX_RCPNE */
	LLDEF|RLDEF,            /* EX_NCPGT */
	LLDEF|RLDEF,            /* EX_NCPGE */
	LLDEF|RLDEF,            /* EX_NCPLT */
	LLDEF|RLDEF,            /* EX_NCPLE */
	LLDEF,                  /* EX_OR2 */
	LLDEF,                  /* EX_AND2 */
	LLDEF|RLDEF,            /* EX_PLUS */
	LLDEF|RLDEF,            /* EX_CAT */
	LLDEF|RLDEF,            /* EX_MINUS */
	LLDEF|RLDEF,            /* EX_TIMES */
	LLDEF|RLDEF,            /* EX_DIVIDE */
	LLDEF|RLDEF,            /* EX_MOD */
	LLDEF|RLDEF,            /* EX_LSHIFT */
	LLDEF|RLDEF,            /* EX_RSHIFT */
	LLDEF,                  /* EX_NOT */
	LLDEF,                  /* EX_COMP */
	0,                      /* EX_COST */
	0,                      /* EX_STRING */
	LLEAF,                  /* EX_DEFINED */
	0,                      /* EX_SUBREG */
	LLDEF,                  /* EX_TOSTRING */
	LLDEF,                  /* EX_UMINUS */
	0,                      /* EX_REG */
	0,			/* EX_LOWW */
	0,			/* EX_HIGHW */
	LLDEF,			/* EX_INREG */
	LLDEF,			/* EX_REGVAR */
};



string mycat(string s1,string s2)
{
	register string s;

	s=salloc(strlen(s1)+strlen(s2));
	strcpy(s,s1);
	strcat(s,s2);
	return(s);
}

string mystrcpy(string s)
{
	register string r;

	r=salloc(strlen(s));
	strcpy(r,s);
	return(r);
}

char digstr[21][15];

string tostring(word n)
{
	char buf[25];

	if (n>=-20 && n<=20 && (n&1)==0) {
		if (digstr[(n>>1)+10][0]==0)
			sprintf(digstr[(n>>1)+10],WRD_FMT,n);
		return(digstr[(n>>1)+10]);
	}
	sprintf(buf,WRD_FMT,n);
	return(mystrcpy(buf));
}

result_t undefres= {EV_UNDEF};

result_t compute(register node_p node)
{
	result_t leaf1,leaf2,result;
	token_p tp;
	int desc;
	long mask,tmp;
	int i,tmpreg;
	glosym_p gp;

	desc=opdesc[node->ex_operator];
	if (desc&LLEAF) {
		leaf1 = compute(&enodes[node->ex_lnode]);
		if (desc&LDEF && leaf1.e_typ==EV_UNDEF)
			return(undefres);
	}
	if (desc&RLEAF) {
		leaf2 = compute(&enodes[node->ex_rnode]);
		if (desc&RDEF && leaf2.e_typ==EV_UNDEF)
			return(undefres);
	}
	result.e_typ=EV_INT;
	switch(node->ex_operator) {
	default:        assert(FALSE);
	case EX_TOKFIELD:
		if (node->ex_lnode!=0)
			tp = &fakestack[stackheight-node->ex_lnode];
		else
			tp = curtoken;
		switch(result.e_typ = tokens[tp->t_token].t_type[node->ex_rnode-1]) {
		default:
			assert(FALSE);
		case EV_INT:
			result.e_v.e_con = tp->t_att[node->ex_rnode-1].aw;
			break;
		case EV_STR:
			result.e_v.e_str = tp->t_att[node->ex_rnode-1].as;
			break;
		case EV_REG:
			result.e_v.e_reg = tp->t_att[node->ex_rnode-1].ar;
			break;
		}
		return(result);
	case EX_ARG:
		return(dollar[node->ex_lnode-1]);
	case EX_CON:
		result.e_typ = EV_INT;
		result.e_v.e_con = ((long) node->ex_rnode << 16) | ((long)node->ex_lnode&0xffff);
		return(result);
	case EX_REG:
		result.e_typ = EV_REG;
		result.e_v.e_reg = node->ex_lnode;
		return(result);
	case EX_ALLREG:
		result.e_typ = EV_REG;
		result.e_v.e_reg = allreg[node->ex_lnode-1];
#if MAXMEMBERS!=0
		if (node->ex_rnode!=0)
			result.e_v.e_reg = machregs[result.e_v.e_reg].
				r_members[node->ex_rnode-1];
#endif
		return(result);
	case EX_SAMESIGN:
	assert(leaf1.e_typ == EV_INT && leaf2.e_typ == EV_INT);
		result.e_typ = EV_INT;
		if (leaf1.e_v.e_con>=0)
			result.e_v.e_con= leaf2.e_v.e_con>=0;
		else
			result.e_v.e_con= leaf2.e_v.e_con<0;
		return(result);
	case EX_SFIT:
	assert(leaf1.e_typ == EV_INT && leaf2.e_typ == EV_INT);
		mask = 0xFFFFFFFFL;
		for (i=0;i<leaf2.e_v.e_con-1;i++)
			mask &= ~(1<<i);
		tmp = leaf1.e_v.e_con&mask;
		result.e_v.e_con = tmp==0||tmp==mask;
		return(result);
	case EX_UFIT:
	assert(leaf1.e_typ == EV_INT && leaf2.e_typ == EV_INT);
		mask = 0xFFFFFFFFL;
		for (i=0;i<leaf2.e_v.e_con;i++)
			mask &= ~(1<<i);
		result.e_v.e_con = (leaf1.e_v.e_con&mask)==0;
		return(result);
	case EX_ROM:
		assert(node->ex_rnode>=0 &&node->ex_rnode<MAXROM);
		leaf2=dollar[node->ex_lnode];
		if (leaf2.e_typ != EV_STR)
			return(undefres);
		gp = lookglo(leaf2.e_v.e_str);
		if (gp == (glosym_p) 0)
			return(undefres);
		if ((gp->gl_rom[MAXROM]&(1<<node->ex_rnode))==0)
			return(undefres);
		result.e_v.e_con = gp->gl_rom[node->ex_rnode];
		return(result);
	case EX_LOWW:
		result.e_v.e_con = saveemp[node->ex_lnode].em_u.em_loper&0xFFFF;
		return(result);
	case EX_HIGHW:
		result.e_v.e_con = saveemp[node->ex_lnode].em_u.em_loper>>16;
		return(result);
	case EX_NCPEQ:
	assert(leaf1.e_typ == EV_INT && leaf2.e_typ == EV_INT);
		result.e_v.e_con = leaf1.e_v.e_con==leaf2.e_v.e_con;
		return(result);
	case EX_SCPEQ:
	assert(leaf1.e_typ == EV_STR && leaf2.e_typ == EV_STR);
		result.e_v.e_con = !strcmp(leaf1.e_v.e_str,leaf2.e_v.e_str);
		return(result);
	case EX_RCPEQ:
	assert(leaf1.e_typ == EV_REG && leaf2.e_typ == EV_REG);
		result.e_v.e_con = leaf1.e_v.e_reg==leaf2.e_v.e_reg;
		return(result);
	case EX_NCPNE:
	assert(leaf1.e_typ == EV_INT && leaf2.e_typ == EV_INT);
		result.e_v.e_con = leaf1.e_v.e_con!=leaf2.e_v.e_con;
		return(result);
	case EX_SCPNE:
	assert(leaf1.e_typ == EV_STR && leaf2.e_typ == EV_STR);
		result.e_v.e_con = strcmp(leaf1.e_v.e_str,leaf2.e_v.e_str);
		return(result);
	case EX_RCPNE:
	assert(leaf1.e_typ == EV_REG && leaf2.e_typ == EV_REG);
		result.e_v.e_con = leaf1.e_v.e_reg!=leaf2.e_v.e_reg;
		return(result);
	case EX_NCPGT:
	assert(leaf1.e_typ == EV_INT && leaf2.e_typ == EV_INT);
		result.e_v.e_con = leaf1.e_v.e_con>leaf2.e_v.e_con;
		return(result);
	case EX_NCPGE:
	assert(leaf1.e_typ == EV_INT && leaf2.e_typ == EV_INT);
		result.e_v.e_con = leaf1.e_v.e_con>=leaf2.e_v.e_con;
		return(result);
	case EX_NCPLT:
	assert(leaf1.e_typ == EV_INT && leaf2.e_typ == EV_INT);
		result.e_v.e_con = leaf1.e_v.e_con<leaf2.e_v.e_con;
		return(result);
	case EX_NCPLE:
	assert(leaf1.e_typ == EV_INT && leaf2.e_typ == EV_INT);
		result.e_v.e_con = leaf1.e_v.e_con<=leaf2.e_v.e_con;
		return(result);
	case EX_OR2:
	assert(leaf1.e_typ == EV_INT);
		if (leaf1.e_v.e_con==0)
			return(compute(&enodes[node->ex_rnode]));
		return(leaf1);
	case EX_AND2:
	assert(leaf1.e_typ == EV_INT);
		if (leaf1.e_v.e_con!=0)
			return(compute(&enodes[node->ex_rnode]));
		return(leaf1);
	case EX_PLUS:
	assert(leaf1.e_typ == EV_INT && leaf2.e_typ == EV_INT);
		result.e_v.e_con=leaf1.e_v.e_con+leaf2.e_v.e_con;
		return(result);
	case EX_CAT:
	assert(leaf1.e_typ == EV_STR && leaf2.e_typ == EV_STR);
		result.e_typ = EV_STR;
		result.e_v.e_str = mycat(leaf1.e_v.e_str,leaf2.e_v.e_str);
		return(result);
	case EX_MINUS:
	assert(leaf1.e_typ == EV_INT && leaf2.e_typ == EV_INT);
		result.e_v.e_con = leaf1.e_v.e_con - leaf2.e_v.e_con;
		return(result);
	case EX_TIMES:
	assert(leaf1.e_typ == EV_INT && leaf2.e_typ == EV_INT);
		result.e_v.e_con = leaf1.e_v.e_con * leaf2.e_v.e_con;
		return(result);
	case EX_DIVIDE:
	assert(leaf1.e_typ == EV_INT && leaf2.e_typ == EV_INT);
		result.e_v.e_con = leaf1.e_v.e_con / leaf2.e_v.e_con;
		return(result);
	case EX_MOD:
	assert(leaf1.e_typ == EV_INT && leaf2.e_typ == EV_INT);
		result.e_v.e_con = leaf1.e_v.e_con % leaf2.e_v.e_con;
		return(result);
	case EX_LSHIFT:
	assert(leaf1.e_typ == EV_INT && leaf2.e_typ == EV_INT);
		result.e_v.e_con = leaf1.e_v.e_con << leaf2.e_v.e_con;
		return(result);
	case EX_RSHIFT:
	assert(leaf1.e_typ == EV_INT && leaf2.e_typ == EV_INT);
		result.e_v.e_con = leaf1.e_v.e_con >> leaf2.e_v.e_con;
		return(result);
	case EX_NOT:
	assert(leaf1.e_typ == EV_INT);
		result.e_v.e_con = !leaf1.e_v.e_con;
		return(result);
	case EX_COMP:
	assert(leaf1.e_typ == EV_INT);
		result.e_v.e_con = ~leaf1.e_v.e_con;
		return(result);
	case EX_COST:
		if (node->ex_rnode==0)
			return(compute(&enodes[tokens[node->ex_lnode].t_cost.c_size]));
		else
			return(compute(&enodes[tokens[node->ex_lnode].t_cost.c_time]));
	case EX_STRING:
		result.e_typ = EV_STR;
		result.e_v.e_str = codestrings[node->ex_lnode];
		return(result);
	case EX_DEFINED:
		result.e_v.e_con=leaf1.e_typ!=EV_UNDEF;
		return(result);
	case EX_SUBREG:
		result.e_typ = EV_REG;
		tp= &fakestack[stackheight-node->ex_lnode];
		assert(tp->t_token == -1);
		tmpreg= tp->t_att[0].ar;
#if MAXMEMBERS!=0
		if (node->ex_rnode)
			tmpreg=machregs[tmpreg].r_members[node->ex_rnode-1];
#endif
		result.e_v.e_reg=tmpreg;
		return(result);
	case EX_TOSTRING:
	assert(leaf1.e_typ == EV_INT);
		result.e_typ = EV_STR;
		result.e_v.e_str = tostring(leaf1.e_v.e_con);
		return(result);
#ifdef REGVARS
	case EX_INREG:
	assert(leaf1.e_typ == EV_INT);
		i = isregvar((long) leaf1.e_v.e_con);
		if (i<0)
			result.e_v.e_con = 0;
		else if (i==0)
			result.e_v.e_con = 1;
		else
			result.e_v.e_con = 2;
		return(result);
	case EX_REGVAR:
	assert(leaf1.e_typ == EV_INT);
		i = isregvar((long) leaf1.e_v.e_con);
		if (i<=0) 
			return(undefres);
		result.e_typ = EV_REG;
		result.e_v.e_reg=i;
		return(result);
#endif
	case EX_UMINUS:
	assert(leaf1.e_typ == EV_INT);
		result.e_v.e_con = -leaf1.e_v.e_con;
		return(result);
	}
}
