#ifndef NORCSID
static char rcsid[] = "$Header$";
#endif

#include "param.h"
#include "types.h"
#include "tes.h"
#include "assert.h"
#include "line.h"
#include "lookup.h"
#include "proinf.h"
#include "alloc.h"
#include "pattern.h"
#include <em_spec.h>
#include <em_mnem.h>
#include "optim.h"
#include "ext.h"

/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 *
 * Author: Hans van Staveren
 */

#undef CHK_HASH	/* print numbers patterns are hashed to */
#ifdef CHK_HASH
#include <stdio.h>
#endif

#define ILLHASH 0177777
short pathash[256];	/* table of indices into pattern[] */

int opind = 0;		/* second index of next matrix */
byte transl[op_plast-op_pfirst+1][3] = {
	/* LLP */	{ op_LLP, op_lol, op_ldl },
	/* LEP */	{ op_LEP, op_loe, op_lde },
	/* SLP */	{ op_SLP, op_stl, op_sdl },
	/* SEP */	{ op_SEP, op_ste, op_sde }
};

opcheck(bp) register byte *bp; {

	if (((*bp)&BMASK) >= op_pfirst)
		*bp = transl[((*bp)&BMASK)-op_pfirst][opind];
}

/*
 * The hashing method used is believed to be reasonably efficient.
 * A minor speed improvement could be obtained by keeping a boolean
 * array telling which opcode has any patterns starting with it.
 * Currently only about one third of the opcodes actually have a
 * pattern starting with it, but they are the most common ones.
 * Estimated improvement possible: about 2%
 */

hashpatterns() {
	short index;
	register byte *bp,*tp;
	register short i;
	unsigned short hashvalue;
	byte *save;
	int patlen;

	if (pointersize == wordsize)
		opind=1;
	else if (pointersize == 2*wordsize)
		opind=2;
	index = lastind;	/* set by mktab */
	while (index != 0) {
		bp = &pattern[index];
		tp = &bp[PO_MATCH];
		i = *tp++&BMASK;
		if (i==BMASK) {
			i = *tp++&BMASK;
			i |= (*tp++&BMASK)<<8;
		}
		save = tp;
		patlen = i;
		while (i--)
			opcheck(tp++);
		if ((*tp++&BMASK)==BMASK)
			tp += 2;
		i = *tp++&BMASK;
		if (i==BMASK) {
			i = *tp++&BMASK;
			i |= (*tp++&BMASK)<<8;
		}
		while (i--) {
			opcheck(tp++);
			if ((*tp++&BMASK)==BMASK)
				tp += 2;
		}

		/*
		 * Now the special opcodes are filled
		 * in properly, we can hash the pattern
		 */

		hashvalue = 0;
		tp = save;
		switch(patlen) {
		default:	/* 3 or more */
			hashvalue = (hashvalue<<4)^(*tp++&BMASK);
		case 2:
			hashvalue = (hashvalue<<4)^(*tp++&BMASK);
		case 1:
			hashvalue = (hashvalue<<4)^(*tp++&BMASK);
		}
		assert(hashvalue!= ILLHASH);
		i=index;
		index = (bp[PO_NEXT]&BMASK)|(bp[PO_NEXT+1]<<8);
		bp[PO_HASH] = hashvalue>>8;
		hashvalue &= BMASK;
		bp[PO_NEXT] = pathash[hashvalue]&BMASK;
		bp[PO_NEXT+1] = pathash[hashvalue]>>8;
		pathash[hashvalue] = i;
#ifdef CHK_HASH
		fprintf(stderr,"%d\n",hashvalue);
#endif
	}
}

peephole() {
	static bool phashed = FALSE;

	if (!phashed) {
		hashpatterns();
		phashed=TRUE;
	}
	return optimize();
}

optimize() {
	register num_p *npp,np;
	register instr;
	bool madeopt;

	madeopt = basicblock(&instrs);
	for (npp=curpro.numhash;npp< &curpro.numhash[NNUMHASH]; npp++)
		for (np = *npp; np != (num_p) 0; np=np->n_next) {
			if (! np->n_line) continue;
			if(np->n_line->l_next == (line_p) 0)
				continue;
			instr = np->n_line->l_next->l_instr&BMASK;
			if (instr == op_lab || instr == op_bra)
				np->n_repl = np->n_line->l_next->l_a.la_np;
			else
				if (basicblock(&np->n_line->l_next))
					madeopt = TRUE;
		}
	return madeopt;
}

offset oabs(off) offset off; {

	return(off >= 0 ? off : -off);
}

line_p repline(ev,patlen) eval_t ev; {
	register line_p lp;
	register iarg_p iap;
	register sym_p sp;
	offset diff,newdiff;

	assert(ev.e_typ != EV_UNDEF);
	switch(ev.e_typ) {
	case EV_CONST:
		if ((short) ev.e_v.e_con == ev.e_v.e_con) {
			if (CANMINI((short) ev.e_v.e_con))
				lp = newline((short) (ev.e_v.e_con)+Z_OPMINI);
			else {
				lp = newline(OPSHORT);
				lp->l_a.la_short = (short) ev.e_v.e_con;
			}
		} else {
			lp = newline(OPOFFSET);
			lp->l_a.la_offset = ev.e_v.e_con;
		}
		return(lp);
	case EV_NUMLAB:
		lp = newline(OPNUMLAB);
		lp->l_a.la_np = ev.e_v.e_np;
		return(lp);
	default:	/* fragment + offset */
		/*
		 * There is a slight problem here, because we have to
		 * map fragment+offset to symbol+offset.
		 * Fortunately the fragment we have must be the fragment
		 * of one of the symbols in the matchpattern.
		 * So a short search should do the job.
		 */
		sp = (sym_p) 0;
		for (iap= &iargs[patlen-1]; iap >= iargs; iap--)
			if (iap->ia_ev.e_typ == ev.e_typ) {
				/*
				 * Although lint complains, diff is not used
				 * before set.
				 *
				 * The proof is left as an exercise to the
				 * reader.
				 */
				newdiff = oabs(iap->ia_sp->s_value-ev.e_v.e_con);
				if (sp==(sym_p) 0 || newdiff < diff) {
					sp = iap->ia_sp;
					diff = newdiff;
				}
			}
		assert(sp != (sym_p) 0);
		if (diff == 0) {
			lp = newline(OPSYMBOL);
			lp->l_a.la_sp = sp;
		} else {
			diff = ev.e_v.e_con - sp->s_value;
			if ((short) diff == diff) {
				lp = newline(OPSVAL);
				lp->l_a.la_sval.lasv_short = (short) diff;
				lp->l_a.la_sval.lasv_sp = sp;
			} else {
				lp = newline(OPLVAL);
				lp->l_a.la_lval.lalv_offset = diff;
				lp->l_a.la_lval.lalv_sp = sp;
			}
		}
		return(lp);
	}
}

offset rotate(w,amount) offset w,amount; {
	offset highmask,lowmask;

#ifndef LONGOFF
	assert(wordsize<=4);
#endif
	highmask = (offset)(-1) << amount;
	lowmask = ~highmask;
	if (wordsize != 4)
		highmask &= wordsize==2 ? 0xFFFF : 0xFF;
	return(((w<<amount)&highmask)|((w>>(8*wordsize-amount))&lowmask));
}

eval_t undefres = { EV_UNDEF };

eval_t compute(pexp) register expr_p pexp; {
	eval_t leaf1,leaf2,res;
	register i;
	register sym_p sp;
	offset mask;

	switch(nparam[pexp->ex_operator]) {
	default:
		assert(FALSE);
	case 2:
		leaf2 = compute(&enodes[pexp->ex_rnode]);
		if (leaf2.e_typ == EV_UNDEF ||
		    nonumlab[pexp->ex_operator] && leaf2.e_typ == EV_NUMLAB ||
		    onlyconst[pexp->ex_operator] && leaf2.e_typ != EV_CONST)
			return(undefres);
	case 1:
		leaf1 = compute(&enodes[pexp->ex_lnode]);
		if (leaf1.e_typ == EV_UNDEF ||
		    nonumlab[pexp->ex_operator] && leaf1.e_typ == EV_NUMLAB ||
		    onlyconst[pexp->ex_operator] && leaf1.e_typ != EV_CONST)
			return(undefres);
	case 0:
		break;
	}

	res.e_typ = EV_CONST;
	res.e_v.e_con = 0;
	switch(pexp->ex_operator) {
	default:
		assert(FALSE);
	case EX_CON:
		res.e_v.e_con = (offset) pexp->ex_lnode;
		break;
	case EX_ARG:
		return(iargs[pexp->ex_lnode - 1].ia_ev);
	case EX_CMPEQ:
		if (leaf1.e_typ != leaf2.e_typ)
			return(undefres);
		if (leaf1.e_typ == EV_NUMLAB) {
			if (leaf1.e_v.e_np == leaf2.e_v.e_np)
				res.e_v.e_con = 1;
			break;
		}
		if (leaf1.e_v.e_con == leaf2.e_v.e_con)
			res.e_v.e_con = 1;
		break;
	case EX_CMPNE:
		if (leaf1.e_typ != leaf2.e_typ) {
			res.e_v.e_con = 1;
			break;
		}
		if (leaf1.e_typ == EV_NUMLAB) {
			if (leaf1.e_v.e_np != leaf2.e_v.e_np)
				res.e_v.e_con = 1;
			break;
		}
		if (leaf1.e_v.e_con != leaf2.e_v.e_con)
			res.e_v.e_con = 1;
		break;
	case EX_CMPGT:
		if (leaf1.e_typ != leaf2.e_typ)
			return(undefres);
		res.e_v.e_con = leaf1.e_v.e_con > leaf2.e_v.e_con;
		break;
	case EX_CMPGE:
		if (leaf1.e_typ != leaf2.e_typ)
			return(undefres);
		res.e_v.e_con = leaf1.e_v.e_con >= leaf2.e_v.e_con;
		break;
	case EX_CMPLT:
		if (leaf1.e_typ != leaf2.e_typ)
			return(undefres);
		res.e_v.e_con = leaf1.e_v.e_con < leaf2.e_v.e_con;
		break;
	case EX_CMPLE:
		if (leaf1.e_typ != leaf2.e_typ)
			return(undefres);
		res.e_v.e_con = leaf1.e_v.e_con <= leaf2.e_v.e_con;
		break;
	case EX_OR2:
		if (leaf1.e_v.e_con != 0)
			return(leaf1);
		leaf2 = compute(&enodes[pexp->ex_rnode]);
		if (leaf2.e_typ != EV_CONST)
			return(undefres);
		return(leaf2);
	case EX_AND2:
		if (leaf1.e_v.e_con == 0)
			return(leaf1);
		leaf2 = compute(&enodes[pexp->ex_rnode]);
		if (leaf2.e_typ != EV_CONST)
			return(undefres);
		return(leaf2);
	case EX_OR1:
		res.e_v.e_con = leaf1.e_v.e_con | leaf2.e_v.e_con;
		break;
	case EX_XOR1:
		res.e_v.e_con = leaf1.e_v.e_con ^ leaf2.e_v.e_con;
		break;
	case EX_AND1:
		res.e_v.e_con = leaf1.e_v.e_con & leaf2.e_v.e_con;
		break;
	case EX_TIMES:
		res.e_v.e_con = leaf1.e_v.e_con * leaf2.e_v.e_con;
		break;
	case EX_DIVIDE:
		res.e_v.e_con = leaf1.e_v.e_con / leaf2.e_v.e_con;
		break;
	case EX_MOD:
		res.e_v.e_con = leaf1.e_v.e_con % leaf2.e_v.e_con;
		break;
	case EX_LSHIFT:
		res.e_v.e_con = leaf1.e_v.e_con << leaf2.e_v.e_con;
		break;
	case EX_RSHIFT:
		res.e_v.e_con = leaf1.e_v.e_con >> leaf2.e_v.e_con;
		break;
	case EX_UMINUS:
		res.e_v.e_con = -leaf1.e_v.e_con;
		break;
	case EX_NOT:
		res.e_v.e_con = !leaf1.e_v.e_con;
		break;
	case EX_COMP:
		res.e_v.e_con = ~leaf1.e_v.e_con;
		break;
	case EX_PLUS:
		if (leaf1.e_typ >= EV_FRAG) {
			if (leaf2.e_typ >= EV_FRAG)
				return(undefres);
			res.e_typ = leaf1.e_typ;
		} else
			res.e_typ = leaf2.e_typ;
		res.e_v.e_con = leaf1.e_v.e_con + leaf2.e_v.e_con;
		break;
	case EX_MINUS:
		if (leaf1.e_typ >= EV_FRAG) {
			if (leaf2.e_typ == EV_CONST)
				res.e_typ = leaf1.e_typ;
			else if (leaf2.e_typ != leaf1.e_typ)
				return(undefres);
		} else if (leaf2.e_typ >= EV_FRAG)
			return(undefres);
		res.e_v.e_con = leaf1.e_v.e_con - leaf2.e_v.e_con;
		break;
	case EX_POINTERSIZE:
		res.e_v.e_con = pointersize;
		break;
	case EX_WORDSIZE:
		res.e_v.e_con = wordsize;
		break;
	case EX_NOTREG:
		res.e_v.e_con = !inreg(leaf1.e_v.e_con);
		break;
	case EX_DEFINED:
		leaf1 = compute(&enodes[pexp->ex_lnode]);
		res.e_v.e_con = leaf1.e_typ != EV_UNDEF;
		break;
	case EX_SAMESIGN:
		res.e_v.e_con = (leaf1.e_v.e_con ^ leaf2.e_v.e_con) >= 0;
		break;
	case EX_ROM:
		if ((sp = iargs[pexp->ex_lnode - 1].ia_sp) != (sym_p) 0 &&
		    sp->s_rom != (offset *) 0) {
			leaf2 = compute(&enodes[pexp->ex_rnode]);
			if (leaf2.e_typ != EV_CONST ||
			    leaf2.e_v.e_con < 0 ||
			    leaf2.e_v.e_con >= MAXROM)
				return(undefres);
			res.e_v.e_con = sp->s_rom[(int)(leaf2.e_v.e_con)];
			break;
		} else
			return(undefres);
	case EX_SFIT:
		mask = 0;
		for (i=leaf2.e_v.e_con - 1;i < 8*sizeof(offset); i++)
			mask |= 1<<i;
		res.e_v.e_con = (leaf1.e_v.e_con&mask) == 0 ||
				       (leaf1.e_v.e_con&mask) == mask;
		break;
	case EX_UFIT:
		mask = 0;
		for (i=leaf2.e_v.e_con;i < 8*sizeof(offset); i++)
			mask |= 1<<i;
		res.e_v.e_con = (leaf1.e_v.e_con&mask) == 0;
		break;
	case EX_ROTATE:
		res.e_v.e_con = rotate(leaf1.e_v.e_con,leaf2.e_v.e_con);
		break;
	}
	return(res);
}

#ifdef ALLOWSPECIAL
extern bool special();
#endif

bool tryrepl(lpp,bp,patlen)
line_p *lpp;
register byte *bp;
int patlen;
{
	int rpllen,instr,rplval;
	register line_p lp;
	line_p replacement,*rlpp,tp;

	rpllen = *bp++&BMASK;
	if (rpllen == BMASK) {
		rpllen = *bp++&BMASK;
		rpllen |= (*bp++&BMASK)<<8;
	}
#ifdef ALLOWSPECIAL
	if (rpllen == 1 && *bp == 0)
		return(special(lpp,bp+1,patlen));
#endif
	replacement = (line_p) 0;
	rlpp = &replacement;
	while (rpllen--) {
		instr = *bp++&BMASK;
		rplval = *bp++&BMASK;
		if (rplval == BMASK) {
			rplval = (*bp++&BMASK);
			rplval |= (*bp++&BMASK)<<8;
		}
		if (rplval)
			lp = repline(compute(&enodes[rplval]),patlen);
		else
			lp = newline(OPNO);

		/*
		 * One replacement instruction is generated,
		 * link in list and proceed with the next one.
		 */

		if (instr == op_lab)
			lp->l_a.la_np->n_line = lp;
		*rlpp = lp;
		rlpp = &lp->l_next;
		lp->l_instr = instr;
	}

	/*
	 * Replace instructions matched by the created replacement
	 */


	OPTIM((bp[0]&BMASK)|(bp[1]&BMASK)<<8);
	for (lp= *lpp;patlen>0;patlen--,tp=lp,lp=lp->l_next)
		;
	tp->l_next = (line_p) 0;
	*rlpp = lp;
	lp = *lpp;
	*lpp = replacement;
	while ( lp != (line_p) 0 ) {
		tp = lp->l_next;
		oldline(lp);
		lp = tp;
	}
	return(TRUE);
}

bool trypat(lpp,bp,len)
line_p *lpp;
register byte *bp;
int len;
{
	register iarg_p iap;
	int i,patlen;
	register line_p lp;
	eval_t result;

	patlen = *bp++&BMASK;
	if (patlen == BMASK) {
		patlen = *bp++&BMASK;
		patlen |= (*bp++&BMASK)<<8;
	}
	if (len == 3) {
		if (patlen<3)
			return(FALSE);
	} else {
		if (patlen != len)
			return(FALSE);
	}

	/*
	 * Length is ok, now check opcodes
	 */

	for (i=0,lp= *lpp;i<patlen && lp != (line_p) 0;i++,lp=lp->l_next)
		if (lp->l_instr != *bp++)
			return(FALSE);
	if (i != patlen)
		return(FALSE);

	/*
	 * opcodes are also correct, now comes the hard part
	 */

	for(i=0,lp= *lpp,iap= iargs; i<patlen;i++,iap++,lp=lp->l_next) {
		switch(lp->l_optyp) {
		case OPNO:
			iap->ia_ev.e_typ = EV_UNDEF;
			break;
		default:
			iap->ia_ev.e_typ = EV_CONST;
			iap->ia_ev.e_v.e_con = (lp->l_optyp&BMASK)-Z_OPMINI;
			break;
		case OPSHORT:
			iap->ia_ev.e_typ = EV_CONST;
			iap->ia_ev.e_v.e_con = lp->l_a.la_short;
			break;
#ifdef LONGOFF
		case OPOFFSET:
			iap->ia_ev.e_typ = EV_CONST;
			iap->ia_ev.e_v.e_con = lp->l_a.la_offset;
			break;
#endif
		case OPNUMLAB:
			iap->ia_ev.e_typ = EV_NUMLAB;
			iap->ia_ev.e_v.e_np = lp->l_a.la_np;
			break;
		case OPSYMBOL:
			iap->ia_ev.e_typ = lp->l_a.la_sp->s_frag;
			iap->ia_sp = lp->l_a.la_sp;
			iap->ia_ev.e_v.e_con = lp->l_a.la_sp->s_value;
			break;
		case OPSVAL:
			iap->ia_ev.e_typ = lp->l_a.la_sval.lasv_sp->s_frag;
			iap->ia_sp = lp->l_a.la_sval.lasv_sp;
			iap->ia_ev.e_v.e_con = lp->l_a.la_sval.lasv_sp->s_value + lp->l_a.la_sval.lasv_short;
			break;
#ifdef LONGOFF
		case OPLVAL:
			iap->ia_ev.e_typ = lp->l_a.la_lval.lalv_sp->s_frag;
			iap->ia_sp = lp->l_a.la_lval.lalv_sp;
			iap->ia_ev.e_v.e_con = lp->l_a.la_lval.lalv_sp->s_value + lp->l_a.la_lval.lalv_offset;
			break;
#endif
		}
	}
	i = *bp++&BMASK;
	if ( i==BMASK ) {
		i = *bp++&BMASK;
		i |= (*bp++&BMASK)<<8;
	}
	if ( i != 0) {
		/* there is a condition */
		result = compute(&enodes[i]);
		if (result.e_typ != EV_CONST || result.e_v.e_con == 0)
			return(FALSE);
	}
	return(tryrepl(lpp,bp,patlen));
}

int
basicblock(alpp) line_p *alpp; {
	register line_p *lpp,lp;
	unsigned short hash[3];
	line_p *next;
	register byte *bp;
	int i;
	short index;
	bool madeopt;
	int count = 0;

	lpp = alpp; madeopt = FALSE;
	while ((*lpp) != (line_p) 0 && ((*lpp)->l_instr&BMASK) != op_lab) {
		lp = *lpp;
		if (repl_muls) {
			line_p b_repl, e_repl;
			int cnt = repl_mul(lp, &b_repl, &e_repl);

			if (cnt > 0 && cnt <= repl_muls) {
				*lpp = b_repl;
				e_repl->l_next = lp->l_next->l_next;
				oldline(lp->l_next);
				oldline(lp);
				lp = b_repl;
				madeopt = TRUE;
			}
			else {
				while (b_repl != (line_p) 0) {
					line_p n = b_repl->l_next;

					oldline(b_repl);
					b_repl = n;
				}
			}
		}

		next = &lp->l_next;
		hash[0] = lp->l_instr&BMASK;
		lp=lp->l_next;
		if (lp != (line_p) 0) {
			hash[1] = (hash[0]<<4)^(lp->l_instr&BMASK);
			lp=lp->l_next;
			if (lp != (line_p) 0)
				hash[2] = (hash[1]<<4)^(lp->l_instr&BMASK);
			else
				hash[2] = ILLHASH;
		} else {
			hash[1] = ILLHASH;
			hash[2] = ILLHASH;
		}

		/*
		 * hashvalues computed. Try for longest pattern first
		 */

		for (i=2;i>=0;i--) {
		    index = pathash[hash[i]&BMASK];
		    while (index != 0) {
			bp = &pattern[index];
			if((bp[PO_HASH]&BMASK) == (hash[i]>>8))
			    if(trypat(lpp,&bp[PO_MATCH],i+1)) {
				madeopt = TRUE;
				next = lpp;
				i = 0;	/* dirty way of double break */
				break;
			    }
			index=(bp[PO_NEXT]&BMASK)|(bp[PO_NEXT+1]<<8);
		    }
		}
		if (lpp == next) {
			count++;
			if (count > 1000) {
				/* probably loop in table */
				fprintf(stderr, "Warning: possible loop in patterns; call an expert\n");
				next = &((*lpp)->l_next);
				count = 0;
			}
		}
		else	count = 0;
		lpp = next;
	}
	return madeopt;
}

repl_mul(lp, b, e)
	register line_p	lp;
	line_p	*b, *e;
{
	register line_p next = lp->l_next;
	int	ins;
	int	sz;
	unsigned long	n;
	int	n0, n1;
  	int	virgin = 1;
	int	retval = 0;

	*b = 0;
	if (! next) return 0;
	if ((ins = (next->l_instr & BMASK)) != op_mli && ins != op_mlu) {
		return 0;
	}
	if ((ins = (lp->l_instr & BMASK)) != op_loc && ins != op_ldc) {
		return 0;
	}
	switch(next->l_optyp) {
	case OPNO:
		return 0;
	case OPSHORT:
		sz = next->l_a.la_short;
		break;
#ifdef LONGOFF
	case OPOFFSET:
		sz = next->l_a.la_offset;
		break;
#endif
	default:
		sz = (next->l_optyp & BMASK) - Z_OPMINI;
		break;
	}
	if (ins == op_loc && sz != wordsize) return 0;
	if (ins == op_ldc && sz != 2*wordsize) return 0;
	if (! repl_longmuls && sz != wordsize) return 0;
	switch(lp->l_optyp) {
	case OPSHORT:
		n = (long) lp->l_a.la_short;
		break;
#ifdef LONGOFF
	case OPOFFSET:
		n = lp->l_a.la_offset;
		break;
#endif
	default:
		n = (long)((lp->l_optyp & BMASK) - Z_OPMINI);
		break;
	}

#define newinstr(res, opcode, val)	(*(res) = newline((short)(val)+Z_OPMINI), (*(res))->l_instr = (opcode))

  	while (n) {
		/* first find "0*1*$" in n */
		for (n1 = 0; n & 1; n>>=1) ++n1;	/* count "1" bits */
		if (n)
			for (n0 = 0; !(n & 1); n>>=1)	/* count "0" bits */
				++n0;
		else
			n0 = 0;

		if (n1 == 0) {
			if (n0) {
				newinstr(b, op_loc, n0); b = &((*b)->l_next);
				newinstr(b, op_slu, sz); b = &((*b)->l_next);
				retval++;
			}
		} else if (n1 == 1) {
			if (virgin) {
				newinstr(b, op_dup, sz); b = &((*b)->l_next);
				virgin = 0;
			}
			else {
				newinstr(b, op_exg, sz); b = &((*b)->l_next);
				newinstr(b, op_dup, 2*sz); b = &((*b)->l_next);
				newinstr(b, op_asp, sz); b = &((*b)->l_next);
				newinstr(b, op_adu, sz); b = &((*b)->l_next);
				newinstr(b, op_exg, sz); b = &((*b)->l_next);
				retval++;
			}
			if (n) {
				newinstr(b, op_loc, n0+n1); b = &((*b)->l_next);
				newinstr(b, op_slu, sz); b = &((*b)->l_next);
				retval++;
			}
		} else {
			if (virgin) {
				newinstr(b, op_dup, sz); b = &((*b)->l_next);
				newinstr(b, op_loc, 0); b = &((*b)->l_next);
				newinstr(b, op_exg, sz); b = &((*b)->l_next);
				virgin = 0;
			}
			else {
				newinstr(b, op_exg, sz); b = &((*b)->l_next);
				newinstr(b, op_dup, 2*sz); b = &((*b)->l_next);
				newinstr(b, op_asp, sz); b = &((*b)->l_next);
				newinstr(b, op_sbu, sz); b = &((*b)->l_next);
				retval++;
				newinstr(b, op_exg, sz); b = &((*b)->l_next);
				newinstr(b, op_loc, n1); b = &((*b)->l_next);
				newinstr(b, op_slu, sz); b = &((*b)->l_next);
				retval++;
				newinstr(b, op_exg, sz); b = &((*b)->l_next);
				newinstr(b, op_dup, 2*sz); b = &((*b)->l_next);
				newinstr(b, op_asp, sz); b = &((*b)->l_next);
				newinstr(b, op_adu, sz); b = &((*b)->l_next);
				newinstr(b, op_exg, sz); b = &((*b)->l_next);
				retval++;
			}
			if (n0) {
				newinstr(b, op_loc, n0); b = &((*b)->l_next);
				newinstr(b, op_slu, sz); b = &((*b)->l_next);
				retval++;
			}
		}
  	}
	newinstr(b, op_asp, sz);
  	if (virgin) {
		b = &((*b)->l_next);
		newinstr(b, sz == wordsize ? op_loc : op_ldc, 0);
  	}
	*e = *b;
	return retval == 0 ? 1 : retval;
#undef newinstr
}
