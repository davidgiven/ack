#ifndef NORCSID
static char rcsid[] = "$Header$";
#endif

#include "param.h"
#include "types.h"
#include "tes.h"
#include "assert.h"
#include <em_spec.h>
#include <em_pseu.h>
#include "alloc.h"
#include "line.h"
#include "lookup.h"
#include "proinf.h"
#include "ext.h"

/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 *
 * Author: Hans van Staveren
 */

process() {

	if (wordsize == 0 || pointersize == 0)
		error("No MES EMX encountered");
	backward();			/* reverse and cleanup list */
	symknown();			/* symbol scope is now known */
	if (!nflag)
		symvalue();		/* give symbols value */
	if (prodepth != 0) {
		if (!nflag) {
		    int npasses = 0;
		    bool madeopt;

		    checklocs();	/* check definition of locals */
		    do {
			madeopt = peephole();	/* local optimization */
			relabel();	/* relabel local labels */
			flow();		/* throw away unreachable code */
		    } while (madeopt && ++npasses < 5000);
		    assert(!madeopt);
		}
		do_tes();		/* top elt. size computation phase */
		outpro();		/* generate PRO pseudo */
		outregs();		/* generate MES ms_reg pseudos */
		outtes();		/* generate MES ms_tes pseudos */
	}
	putlines(pseudos);		/* pseudos first */
	if (prodepth != 0) {
		putlines(instrs);	/* instructions next */
		outend();		/* generate END pseudo */
		cleanlocals();		/* forget instruction labels */
	} else if(instrs != (line_p) 0)
		error("instructions outside procedure");
#ifdef COREDEBUG
	coreverbose();
#endif
}

relabel() {
	register num_p *npp,np,tp;
	register num_p repl,ttp;

	/*
	 * For each label find its final destination after crossjumping.
	 * Care has to be taken to prevent a loop in the program to
	 * cause same in the optimizer.
	 */

	for (npp = curpro.numhash; npp < &curpro.numhash[NNUMHASH]; npp++)
		for (np = *npp; np != (num_p) 0; np = np->n_next) {
			assert(! np->n_line ||
			   ((np->n_line->l_instr&BMASK) == op_lab
			    && np->n_line->l_a.la_np == np));
			for(tp=np; (tp->n_flags&(NUMKNOWN|NUMMARK))==0;
				   tp = tp->n_repl)
				tp->n_flags |= NUMMARK;
			repl = tp->n_repl;
			for(tp=np; tp->n_flags&NUMMARK; tp = ttp) {
				ttp = tp->n_repl;
				tp->n_repl = repl;
				tp->n_flags &= ~ NUMMARK;
				tp->n_flags |=   NUMKNOWN;
			}
		}
	for (npp = curpro.numhash; npp < &curpro.numhash[NNUMHASH]; npp++)
		for (np = *npp; np != (num_p) 0; np = np->n_next) {
			np->n_flags &= ~(NUMKNOWN|NUMSCAN|NUMREACH);
			np->n_jumps = 0;
		}
}

symknown() {
	register sym_p *spp,sp;

	for (spp = symhash; spp < &symhash[NSYMHASH]; spp++)
		for (sp = *spp; sp != (sym_p) 0; sp = sp->s_next)
			if (sp->s_flags & SYMSEEN)
				sp->s_flags |= SYMKNOWN;
}

cleanlocals() {
	register num_p *npp,np,tp;

	for (npp = curpro.numhash; npp < &curpro.numhash[NNUMHASH]; npp++) {
		np = *npp;
		while (np != (num_p) 0) {
			tp = np->n_next;
			oldnum(np);
			np = tp;
		}
		*npp = (num_p) 0;
	}
}

checklocs() {
	register num_p *npp,np;

	for (npp=curpro.numhash; npp < & curpro.numhash[NNUMHASH]; npp++)
		for (np = *npp; np != (num_p) 0; np=np->n_next)
			if (np->n_line == (line_p) 0)
				error("local label %u undefined",
					(unsigned) np->n_number);
}

offset align(count,alignment) offset count,alignment; {

	assert(alignment==1||alignment==2||alignment==4);
	return((count+alignment-1)&~(alignment-1));
}

symvalue() {
	register line_p lp;
	register sym_p sp;
	register arg_p ap;
	register argb_p abp;
	short curfrag = 0;
	offset count;

	for (lp=pseudos; lp != (line_p) 0; lp = lp->l_next)
	switch(lp->l_instr&BMASK) {
	default:
		assert(FALSE);
	case ps_sym:
		sp = lp->l_a.la_sp;
		if (sp->s_frag != curfrag) {
			count = 0;
			curfrag = sp->s_frag;
		}
		count = align(count,wordsize);
		sp->s_value = count;
		break;
	case ps_bss:
	case ps_hol:
		/* nothing to do, all bss pseudos are in diff frags */
	case ps_mes:
		break;
	case ps_con:
	case ps_rom:
		for (ap=lp->l_a.la_arg; ap  != (arg_p) 0; ap = ap->a_next)
		switch(ap->a_typ) {
		default:
			assert(FALSE);
		case ARGOFF:
			count = align(count,wordsize)+wordsize;
			break;
		case ARGNUM:
		case ARGSYM:
		case ARGVAL:
			count = align(count,wordsize)+pointersize;
			break;
		case ARGICN:
		case ARGUCN:
		case ARGFCN:
			if (ap->a_a.a_con.ac_length < wordsize)
				count = align(count,(offset)ap->a_a.a_con.ac_length);
			else
				count = align(count,wordsize);
			count += ap->a_a.a_con.ac_length;
			break;
		case ARGSTR:
			for (abp = &ap->a_a.a_string; abp != (argb_p) 0;
			     abp = abp->ab_next)
				count += abp->ab_index;
			break;
		}
	}
}

do_tes()
{
	register line_p insptr = instrs, oldlin = NULL, oldlin2 = NULL;

	init_state();
	tes_pseudos();
	while (insptr != NULL) {
		tes_instr(insptr, oldlin, oldlin2);
		oldlin2 = oldlin;
		oldlin = insptr;
		insptr = insptr->l_next;
	}
}
