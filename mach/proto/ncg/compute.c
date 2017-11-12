#ifndef NORCSID
static char rcsid[] = "$Id$";
#endif

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "param.h"
#include "tables.h"
#include "types.h"
#include <cgg_cg.h>
#include "data.h"
#include "result.h"
#include "glosym.h"
#include "extern.h"
#ifdef REGVARS
#include "regvar.h"
#endif
#ifdef USE_TES
#include "label.h"
#endif

/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 *
 * Author: Hans van Staveren
 */

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
	LLDEF|RLDEF,		/* EX_OR */
	LLDEF|RLDEF,		/* EX_XOR */
	LLDEF|RLDEF,		/* EX_AND */
	0,			/* EX_ISROM */
#ifdef USE_TES
	0,			/* EX_TOPELTSIZE */
	0,			/* EX_FALLTHROUGH */
#endif
};

static string mycat(string s1, string s2) {
	string s;

	if (s1==0 || *s1=='\0') return(s2);
	if (s2==0 || *s2=='\0') return(s1);
	s=salloc(strlen(s1)+strlen(s2)+1);
	strcpy(s,s1);
	strcat(s,"+");
	strcat(s,s2);
	return(s);
}

string mystrcpy(string s) {
	register string r;

	r=salloc(strlen(s));
	strcpy(r,s);
	return(r);
}

static char digstr[21][15];

string tostring(word n) {
	char buf[25];

	if (n>=-20 && n<=20 && (n&1)==0) {
		if (digstr[((int)n>>1)+10][0]==0)
			sprintf(digstr[((int)n>>1)+10],WRD_FMT,n);
		return(digstr[((int)n>>1)+10]);
	}
	sprintf(buf,WRD_FMT,n);
	return(mystrcpy(buf));
}

void
compute(node, presult) register node_p node; register result_t *presult; {
	result_t leaf1,leaf2;
	register token_p tp;
	int desc;
	long mask,tmp;
	int i,tmpreg;
	glosym_p gp;

	presult->e_typ = EV_UNDEF;
	desc=opdesc[node->ex_operator];
	if (desc&LLEAF) {
		compute(&enodes[node->ex_lnode], &leaf1);
		if (desc&LDEF && leaf1.e_typ==EV_UNDEF)
			return;
	}
	if (desc&RLEAF) {
		compute(&enodes[node->ex_rnode], &leaf2);
		if (desc&RDEF && leaf2.e_typ==EV_UNDEF)
			return;
	}
	presult->e_typ=EV_INT;
	switch(node->ex_operator) {
	default:        assert(FALSE);
	case EX_TOKFIELD:
		if (node->ex_lnode==0)
			if (curtoken) tp = curtoken;
			else tp = &fakestack[stackheight-1];
		else	tp = &fakestack[stackheight-node->ex_lnode];
		switch(presult->e_typ = tokens[tp->t_token].t_type[node->ex_rnode-1]) {
		default:
			assert(FALSE);
		case EV_INT:
			presult->e_v.e_con = tp->t_att[node->ex_rnode-1].aw;
			break;
		case EV_ADDR:
			presult->e_v.e_addr = tp->t_att[node->ex_rnode-1].aa;
			break;
		case EV_REG:
			presult->e_v.e_reg = tp->t_att[node->ex_rnode-1].ar;
			break;
		}
		return;
	case EX_ARG:
		*presult = dollar[node->ex_lnode-1];
		return;
	case EX_CON:
		presult->e_typ = EV_INT;
		presult->e_v.e_con = ((long) node->ex_rnode << 16) | ((long)node->ex_lnode&0xffff);
		return;
	case EX_REG:
		presult->e_typ = EV_REG;
		presult->e_v.e_reg = node->ex_lnode;
		return;
	case EX_ALLREG:
		presult->e_typ = EV_REG;
		presult->e_v.e_reg = allreg[node->ex_lnode-1];
#if MAXMEMBERS!=0
		if (node->ex_rnode!=0)
			presult->e_v.e_reg = machregs[presult->e_v.e_reg].
				r_members[node->ex_rnode-1];
#endif
		return;
	case EX_SAMESIGN:
	assert(leaf1.e_typ == EV_INT && leaf2.e_typ == EV_INT);
		presult->e_typ = EV_INT;
		if (leaf1.e_v.e_con>=0)
			presult->e_v.e_con= leaf2.e_v.e_con>=0;
		else
			presult->e_v.e_con= leaf2.e_v.e_con<0;
		return;
	case EX_SFIT:
	assert(leaf1.e_typ == EV_INT && leaf2.e_typ == EV_INT);
		mask = 0xFFFFFFFFL;
		for (i=0;i<leaf2.e_v.e_con-1;i++)
			mask &= ~(1<<i);
		tmp = leaf1.e_v.e_con&mask;
		presult->e_v.e_con = tmp==0||tmp==mask;
		return;
	case EX_UFIT:
	assert(leaf1.e_typ == EV_INT && leaf2.e_typ == EV_INT);
		mask = 0xFFFFFFFFL;
		for (i=0;i<leaf2.e_v.e_con;i++)
			mask &= ~(1<<i);
		presult->e_v.e_con = (leaf1.e_v.e_con&mask)==0;
		return;
	case EX_ROM:
		assert(node->ex_rnode>=0 &&node->ex_rnode<MAXROM);
		leaf2=dollar[node->ex_lnode];
		presult->e_typ = EV_UNDEF;
		if (leaf2.e_typ != EV_ADDR)
			return;
		if (leaf2.e_v.e_addr.ea_off!=0)
			return;
		gp = lookglo(leaf2.e_v.e_addr.ea_str);
		if (gp == (glosym_p) 0)
			return;
		if ((gp->gl_rom[MAXROM]&(1<<node->ex_rnode))==0)
			return;
		presult->e_typ = EV_INT;
		presult->e_v.e_con = gp->gl_rom[node->ex_rnode];
		return;
	case EX_ISROM:
		leaf2=dollar[node->ex_lnode];
		if (leaf2.e_typ != EV_ADDR)
			presult->e_v.e_con = 0;
		else
			presult->e_v.e_con = lookglo(leaf2.e_v.e_addr.ea_str) != 0;
		return;
	case EX_LOWW:
		presult->e_v.e_con = saveemp[node->ex_lnode].em_u.em_loper&0xFFFF;
		return;
	case EX_HIGHW:
		presult->e_v.e_con = saveemp[node->ex_lnode].em_u.em_loper>>16;
		return;
	case EX_NCPEQ:
	assert(leaf1.e_typ == EV_INT && leaf2.e_typ == EV_INT);
		presult->e_v.e_con = leaf1.e_v.e_con==leaf2.e_v.e_con;
		return;
	case EX_SCPEQ:
	assert(leaf1.e_typ == EV_ADDR && leaf2.e_typ == EV_ADDR);
		presult->e_v.e_con =
		    (strcmp(leaf1.e_v.e_addr.ea_str,leaf2.e_v.e_addr.ea_str)==0 &&
		    leaf1.e_v.e_addr.ea_off==leaf2.e_v.e_addr.ea_off);
		return;
	case EX_RCPEQ:
	assert(leaf1.e_typ == EV_REG && leaf2.e_typ == EV_REG);
		presult->e_v.e_con = leaf1.e_v.e_reg==leaf2.e_v.e_reg;
		return;
	case EX_NCPNE:
	assert(leaf1.e_typ == EV_INT && leaf2.e_typ == EV_INT);
		presult->e_v.e_con = leaf1.e_v.e_con!=leaf2.e_v.e_con;
		return;
	case EX_SCPNE:
	assert(leaf1.e_typ == EV_ADDR && leaf2.e_typ == EV_ADDR);
		presult->e_v.e_con =
		    !(strcmp(leaf1.e_v.e_addr.ea_str,leaf2.e_v.e_addr.ea_str)==0 &&
		    leaf1.e_v.e_addr.ea_off==leaf2.e_v.e_addr.ea_off);
		return;
	case EX_RCPNE:
	assert(leaf1.e_typ == EV_REG && leaf2.e_typ == EV_REG);
		presult->e_v.e_con = leaf1.e_v.e_reg!=leaf2.e_v.e_reg;
		return;
	case EX_NCPGT:
	assert(leaf1.e_typ == EV_INT && leaf2.e_typ == EV_INT);
		presult->e_v.e_con = leaf1.e_v.e_con>leaf2.e_v.e_con;
		return;
	case EX_NCPGE:
	assert(leaf1.e_typ == EV_INT && leaf2.e_typ == EV_INT);
		presult->e_v.e_con = leaf1.e_v.e_con>=leaf2.e_v.e_con;
		return;
	case EX_NCPLT:
	assert(leaf1.e_typ == EV_INT && leaf2.e_typ == EV_INT);
		presult->e_v.e_con = leaf1.e_v.e_con<leaf2.e_v.e_con;
		return;
	case EX_NCPLE:
	assert(leaf1.e_typ == EV_INT && leaf2.e_typ == EV_INT);
		presult->e_v.e_con = leaf1.e_v.e_con<=leaf2.e_v.e_con;
		return;
	case EX_OR2:
	assert(leaf1.e_typ == EV_INT);
		if (leaf1.e_v.e_con==0) {
			compute(&enodes[node->ex_rnode], presult);
		}
		else	*presult = leaf1;
		return;
	case EX_AND2:
	assert(leaf1.e_typ == EV_INT);
		if (leaf1.e_v.e_con!=0) {
			compute(&enodes[node->ex_rnode], presult);
		}
		else	*presult = leaf1;
		return;
	case EX_PLUS:
	assert(leaf1.e_typ == EV_INT && leaf2.e_typ == EV_INT);
		presult->e_v.e_con=leaf1.e_v.e_con+leaf2.e_v.e_con;
		return;
	case EX_CAT:
	assert(leaf1.e_typ == EV_ADDR && leaf2.e_typ == EV_ADDR);
		presult->e_typ = EV_ADDR;
		presult->e_v.e_addr.ea_str = mycat(leaf1.e_v.e_addr.ea_str,leaf2.e_v.e_addr.ea_str);
		presult->e_v.e_addr.ea_off = leaf1.e_v.e_addr.ea_off+leaf2.e_v.e_addr.ea_off;
		return;
	case EX_MINUS:
	assert(leaf1.e_typ == EV_INT && leaf2.e_typ == EV_INT);
		presult->e_v.e_con = leaf1.e_v.e_con - leaf2.e_v.e_con;
		return;
	case EX_OR:
	assert(leaf1.e_typ == EV_INT && leaf2.e_typ == EV_INT);
		presult->e_v.e_con = leaf1.e_v.e_con | leaf2.e_v.e_con;
		return;
	case EX_XOR:
	assert(leaf1.e_typ == EV_INT && leaf2.e_typ == EV_INT);
		presult->e_v.e_con = leaf1.e_v.e_con ^ leaf2.e_v.e_con;
		return;
	case EX_AND:
	assert(leaf1.e_typ == EV_INT && leaf2.e_typ == EV_INT);
		presult->e_v.e_con = leaf1.e_v.e_con & leaf2.e_v.e_con;
		return;
	case EX_TIMES:
	assert(leaf1.e_typ == EV_INT && leaf2.e_typ == EV_INT);
		presult->e_v.e_con = leaf1.e_v.e_con * leaf2.e_v.e_con;
		return;
	case EX_DIVIDE:
	assert(leaf1.e_typ == EV_INT && leaf2.e_typ == EV_INT);
		presult->e_v.e_con = leaf1.e_v.e_con / leaf2.e_v.e_con;
		return;
	case EX_MOD:
	assert(leaf1.e_typ == EV_INT && leaf2.e_typ == EV_INT);
		presult->e_v.e_con = leaf1.e_v.e_con % leaf2.e_v.e_con;
		return;
	case EX_LSHIFT:
	assert(leaf1.e_typ == EV_INT && leaf2.e_typ == EV_INT);
		presult->e_v.e_con = leaf1.e_v.e_con << leaf2.e_v.e_con;
		return;
	case EX_RSHIFT:
	assert(leaf1.e_typ == EV_INT && leaf2.e_typ == EV_INT);
		presult->e_v.e_con = leaf1.e_v.e_con >> leaf2.e_v.e_con;
		return;
	case EX_NOT:
	assert(leaf1.e_typ == EV_INT);
		presult->e_v.e_con = !leaf1.e_v.e_con;
		return;
	case EX_COMP:
	assert(leaf1.e_typ == EV_INT);
		presult->e_v.e_con = ~leaf1.e_v.e_con;
		return;
	case EX_STRING:
		presult->e_typ = EV_ADDR;
		presult->e_v.e_addr.ea_str = codestrings[node->ex_lnode];
		presult->e_v.e_addr.ea_off = 0;
		return;
	case EX_DEFINED:
		presult->e_v.e_con=leaf1.e_typ!=EV_UNDEF;
		return;
	case EX_SUBREG:
		presult->e_typ = EV_REG;
		if (node->ex_lnode==0)
			if (curtoken) tp = curtoken;
			else tp = &fakestack[stackheight-1];
		else	tp = &fakestack[stackheight-node->ex_lnode];
		assert(tp->t_token == -1);
		tmpreg= tp->t_att[0].ar;
#if MAXMEMBERS!=0
		if (node->ex_rnode)
			tmpreg=machregs[tmpreg].r_members[node->ex_rnode-1];
#endif
		presult->e_v.e_reg=tmpreg;
		return;
	case EX_TOSTRING:
	assert(leaf1.e_typ == EV_INT);
		presult->e_typ = EV_ADDR;
		presult->e_v.e_addr.ea_str = "";
		presult->e_v.e_addr.ea_off = leaf1.e_v.e_con;
		return;
#ifdef REGVARS
	case EX_INREG:
	assert(leaf1.e_typ == EV_INT);
		presult->e_v.e_con = isregtyp((long) leaf1.e_v.e_con);
		return;
	case EX_REGVAR:
	assert(leaf1.e_typ == EV_INT);
		i = PICK_REGVAR((long) leaf1.e_v.e_con, node->ex_rnode);
		if (i<=0)  {
			presult->e_typ = EV_UNDEF;
			return;
		}
		presult->e_typ = EV_REG;
		presult->e_v.e_reg=i;
		return;
#endif /* REGVARS */
	case EX_UMINUS:
	assert(leaf1.e_typ == EV_INT);
		presult->e_v.e_con = -leaf1.e_v.e_con;
		return;
#ifdef USE_TES
	case EX_TOPELTSIZE:			/* Hans, new */
	    { register label_p lbl;

		lbl = get_label(saveemp[node->ex_lnode].em_u.em_ioper);
		if (lbl != (label_p)0) {
		    presult->e_v.e_con = lbl->lb_height;
		} else {
		    presult->e_v.e_con = 0;
		}
		return;
	    }
	case EX_FALLTHROUGH:			/* Hans, new */
	    { register label_p lbl;

		lbl = get_label(saveemp[node->ex_lnode].em_u.em_ioper);
		if (lbl != (label_p)0) {
		    presult->e_v.e_con = lbl->lb_fallthrough;
		} else presult->e_v.e_con = 0;
		return;
	    }
#endif
	}
}
