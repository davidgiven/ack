/* $Header$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/*  C O N T R O L   F L O W
 *
 *  M A I N   R O U T I N E
 */

#include <stdio.h>
#include <em_mnem.h>
#include <em_pseu.h>
#include <em_spec.h>
#include <em_flag.h>
#include <em_mes.h>
#include "../share/types.h"
#include "../share/debug.h"
#include "../share/map.h"
#include "../share/files.h"
#include "../share/global.h"
#include "../share/alloc.h"
#include "../share/lset.h"
#include "../share/cset.h"
#include "../share/get.h"
#include "../share/put.h"
#include "../share/def.h"
#include "cf.h"
#include "cf_succ.h"
#include "cf_idom.h"
#include "cf_loop.h"

#define newcfbx()	(bext_p) newstruct(bext_cf)
#define oldcfbx(x)	oldstruct(bext_cf,x)

extern char em_flag[];

STATIC cset	lpi_set;	/* set of procedures used in LPI instruction */
STATIC cset	cai_set;	/* set of all procedures doing a CAI */


/* The procedure getbblocks reads the EM textfile and 
 * partitions every procedure into a number of basic blocks.
 */

#define LABEL0		0
#define LABEL		1
#define NORMAL		2
#define JUMP		3
#define END		4
#define AFTERPRO	5
#define INIT		6


/* These global variables are used by getbblocks and nextblock. */

STATIC bblock_p b, *bp;  /* b is the current basic block, bp is
			  * the address where the next block has
			  * to be linked.
			  */
STATIC line_p   lnp, *lp; /* lnp is the current line, lp is
			   * the address where the next line
			   * has to be linked.
			   */
STATIC short state;	/* We use a finite state machine with the
			 * following states:
			 *  LABEL0: after the first (successive)
			 *	    instruction label.
			 *  LABEL1:  after at least two successive
			 *	    instruction labels.
			 *  NORMAL: after a normal instruction.
			 *  JUMP:   after a branch (conditional,
			 *	    unconditional or CSA/CSB).
			 *  END:    after an END pseudo
			 *  AFTERPRO: after we've read a PRO pseudo
			 *  INIT:   initial state
			 */


STATIC nextblock()
{
	/* allocate a new basic block structure and
	 * set b, bp and lp.
	 */

	b = *bp = freshblock();
	bp = &b->b_next;
	b->b_start = lnp;
	b->b_succ = Lempty_set();
	b->b_pred = Lempty_set();
	b->b_extend = newcfbx(); /* basic block extension for CF */
	b->b_extend->bx_cf.bx_bucket = Lempty_set();
	b->b_extend->bx_cf.bx_semi = 0;
	lp = &lnp->l_next;
#ifdef TRACE
	fprintf(stderr,"new basic block, id = %d\n",lastbid);
#endif
}


STATIC short kind(lnp)
	line_p lnp;
{
	/* determine if lnp is a label, branch, end or otherwise */

	short instr;
	byte  flow;

	if ((instr = INSTR(lnp)) == op_lab) return (short) LABEL;
	if (instr == ps_end) return (short) END;
	if (instr > sp_lmnem) return (short) NORMAL; /* pseudo */
	if ((flow = (em_flag[instr-sp_fmnem] & EM_FLO)) == FLO_C ||
	     flow == FLO_T) return (short) JUMP; /* conditional/uncond. jump */
	return (short) NORMAL;
}


STATIC line_p doread_line(p_out)
	proc_p *p_out;
{
	/* read a line, and check pseudos for procedure addresses */

	register line_p lnp = read_line(p_out);

	if (lnp && TYPE(lnp) == OPLIST && INSTR(lnp) != ps_mes) {
		register arg_p arg = ARG(lnp);
		
		while (arg) {
			if (arg->a_type == ARGPROC) {
				Cadd(arg->a_a.a_proc->p_id, &lpi_set);
				arg->a_a.a_proc->p_flags1 |= PF_LPI;
			}
			arg = arg->a_next;
		}
	}
	return lnp;
}

STATIC bool getbblocks(fp,kind_out,n_out,g_out,l_out)
	FILE *fp;
	short *kind_out;
	short *n_out;
	bblock_p *g_out;
	line_p *l_out;
{
	bblock_p head = (bblock_p) 0;
	line_p headl = (line_p) 0;

	curproc = (proc_p) 0;
	/* curproc will get a value when we encounter a PRO pseudo.
	 * If there is no such pseudo, we're reading only data
	 * declarations or messages (outside any proc.).
	 */
	curinp = fp;
	lastbid = (block_id) 0;  /* block identier */
	state = INIT;	/* initial state */
	bp = &head;

	for (;;) {
#ifdef TRACE
		fprintf(stderr,"state = %d\n",state);
#endif
		switch(state) {
			case LABEL0:
				nextblock();
				/* Fall through !! */
			case LABEL:
				lbmap[INSTRLAB(lnp)] = b;
				/* The lbmap table contains for each
				 * label_id the basic block of that label.
				 */
				lnp = doread_line(&curproc);
				state = kind(lnp);
				if (state != END) {
					*lp = lnp;
					lp = &lnp->l_next;
				}
				break;
			case NORMAL:
				lnp = doread_line(&curproc);
				if ( (state = kind(lnp)) == LABEL) {
					/* If we come accross a label
					 * here, it must be the beginning
					 * of a new basic block.
					 */
					state = LABEL0;
				} else {
					if (state != END) {
						*lp = lnp;
						lp = &lnp->l_next;
					}
				}
				break;
			case JUMP:
				lnp = doread_line(&curproc);
				/* fall through ... */
			case AFTERPRO:
				switch(state = kind(lnp)) {
					case LABEL:
						state = LABEL0;
						break;
					case JUMP:
					case NORMAL:
						nextblock();
						break;
				}
				break;
			case END:
				*lp = lnp;
#ifdef TRACE
				fprintf(stderr,"at end of proc, %d blocks\n",lastbid);
#endif
				if (head == (bblock_p) 0) {
					*kind_out = LDATA;
					*l_out = headl;
				} else {
					*kind_out = LTEXT;
					*g_out = head;
					*n_out = (short) lastbid;
					/* number of basic blocks */
				}
				return TRUE;
			case INIT:
				lnp = doread_line(&curproc);
				if (feof(curinp)) return FALSE;
				if (INSTR(lnp) == ps_pro) {
					state = AFTERPRO;
				} else {
					state = NORMAL;
					headl = lnp;
					lp = &lnp->l_next;
				}
				break;
		}
	}
}


STATIC interproc_analysis(p)
	proc_p p;
{
	/* Interprocedural analysis of a procedure p determines:
	 *  - all procedures called by p (the 'call graph')
	 *  - the set of objects changed by p (directly)
	 *  - whether p does a load-indirect (loi,lof etc.)
	 *  - whether p does a store-indirect (sti, stf etc.)
	 * The changed/used variables information will be
	 * transitively closed, i.e. if P calls Q and Q changes
	 * a variable X, the P changes X too.
	 * (The same applies for used variables and for use/store
	 * indirect).
	 * The transitive closure will be computed by main
	 * after all procedures have been processed.
	 */

	bblock_p b;
	line_p   lnp;
	bool inloop;

	/* Allocate memory for structs and sets */

	p->p_use = newuse();
	p->p_change = newchange();
	p->p_change->c_ext = Cempty_set(olength);
	p->p_calling = Cempty_set(plength);

	for (b = p->p_start; b != (bblock_p) 0; b = b->b_next) {
	   inloop = (Lnrelems(b->b_loops) > 0);
	   for (lnp = b->b_start; lnp != (line_p) 0; lnp = lnp->l_next) {
		/* for all instructions of p do */
		switch(INSTR(lnp)) {
		   case op_cal:
			Cadd(PROC(lnp)->p_id, &p->p_calling);
			/* add called proc to p_calling */
			if (inloop) {
				CALLED_IN_LOOP(PROC(lnp));
			}
			break;
		   case op_cai:
			Cadd(p->p_id,&cai_set);
			break;
		   case op_lpi:
			Cadd(PROC(lnp)->p_id, &lpi_set);
			/* All procedures that have their names used
			 * in an lpi instruction, may be called via
			 * a cai instruction.
			 */
			PROC(lnp)->p_flags1 |= PF_LPI;
			break;
		   case op_ste:
		   case op_sde:
		   case op_ine:
		   case op_dee:
		   case op_zre:
			Cadd(OBJ(lnp)->o_id, &p->p_change->c_ext);
			/* Add changed object to c_ext */
			break;
		   case op_lil:
		   case op_lof:
		   case op_loi:
		   case op_los:
		   case op_lar:
			p->p_use->u_flags |= UF_INDIR;
			/* p does a load-indirect */
			break;
		   case op_sil:
		   case op_stf:
		   case op_sti:
		   case op_sts:
		   case op_sar:
			p->p_change->c_flags |= CF_INDIR;
			/* p does a store-indirect */
			break;
		   case op_blm:
		   case op_bls:
			p->p_use->u_flags |= UF_INDIR;
			p->p_change->c_flags |= CF_INDIR;
			/* p does both */
			break;
		   case op_mon:
			printf("mon not yet implemented\n");
			break;
		   case op_lxl:
		   case op_lxa:
			curproc->p_flags1 |= PF_ENVIRON;
			break;
		   case op_lor:
		   case op_str:
			if (SHORT(lnp) == 0) {
				curproc->p_flags1 |= PF_ENVIRON;
			}
			break;
		   case ps_mes:
			if (aoff(ARG(lnp),0) == ms_gto) {
				ENTERED_WITH_GTO(curproc);
			}
			break;
		}
	   }
	}
}


STATIC cf_cleanproc(p)
	proc_p p;
{
	/* Remove the extended data structures of p */

	register bblock_p b;
	register Lindex pi;
	loop_p lp;

	for (b = p->p_start; b != (bblock_p) 0; b = b->b_next) {
		oldcfbx(b->b_extend);
	}
	for (pi = Lfirst(p->p_loops); pi != (Lindex) 0; pi = Lnext(pi,
							p->p_loops)) {
		lp = (loop_p) Lelem(pi);
		oldcflpx(lp->lp_extend);
	}
}



#define CH_CHANGE_INDIR(ch)	((ch->c_flags & CF_INDIR) != 0)
#define USE_INDIR(us)		((us->u_flags & UF_INDIR) != 0)
#define CALLS_UNKNOWN(p)	(p->p_flags1 & (byte) PF_CALUNKNOWN)
#define ENVIRON(p)		(p->p_flags1 & (byte) PF_ENVIRON)


STATIC bool add_info(q,p)
	proc_p q,p;
{
	/* Determine the consequences for used/changed variables info
	 * of the fact that p calls q. If e.g. q changes a variable X
	 * then p changes this variable too. This routine is an
	 * auxiliary routine of the transitive closure process.
	 * The returned value indicates if there was any change in
	 * the information of p.
	 */

	change_p chp, chq;
	use_p    usp, usq;
	bool     diff = FALSE;

	chp = p->p_change;
	chq = q->p_change;
	usp = p->p_use;
	usq = q->p_use;

	if (!BODY_KNOWN(q)) {
		/* q is a procedure of which the body is not available
		 * as EM text.
		 */
		if (CALLS_UNKNOWN(p)) {
			return FALSE;
			/* p already called an unknown procedure */
		} else {
			p->p_flags1 |= PF_CALUNKNOWN;
			return TRUE;
		}
	}
	if (CALLS_UNKNOWN(q)) {
		/* q calls a procedure of which the body is not available
		 * as EM text.
		 */
		if (!CALLS_UNKNOWN(p)) {
			p->p_flags1 |= PF_CALUNKNOWN;
			diff = TRUE;
		}
	}
	if (IS_CALLED_IN_LOOP(p) && !IS_CALLED_IN_LOOP(q)) {
		CALLED_IN_LOOP(q);
		diff = TRUE;
	}
	if (!Cis_subset(chq->c_ext, chp->c_ext)) {
		/* q changes global variables (objects) that
		* p did not (yet) change. Add all variables
		* changed by q to the c_ext set of p.
		*/
		Cjoin(chq->c_ext, &chp->c_ext);
		diff = TRUE;
	}
	if (CH_CHANGE_INDIR(chq) && !CH_CHANGE_INDIR(chp)) {
		/* q does a change-indirect (sil etc.)
		 * and p did not (yet).
		 */
		chp->c_flags |= CF_INDIR;
		diff = TRUE;
	}
	if (USE_INDIR(usq) && !USE_INDIR(usp)) {
		/* q does a use-indirect (lil etc.)
		 * and p dis not (yet).
		 */
		usp->u_flags |= UF_INDIR;
		diff = TRUE;
	}
	if (ENVIRON(q) && !ENVIRON(p)) {
		/* q uses or changes local variables in its
		 * environment while p does not (yet).
		 */
		p->p_flags1 |= PF_ENVIRON;
		diff = TRUE;
	}
	return diff;
}



STATIC trans_clos(head)
	proc_p head;
{
	/* Compute the transitive closure of the used/changed
	 * variable information.
	 */

	register proc_p p,q;
	Cindex i;
	bool changes = TRUE;

	while(changes) {
		changes = FALSE;
		for (p = head; p != (proc_p) 0; p = p->p_next) {
		   if (!BODY_KNOWN(p)) continue;
		   for (i = Cfirst(p->p_calling); i != (Cindex) 0;
						i = Cnext(i,p->p_calling)) {
			q = pmap[Celem(i)];
			if (add_info(q,p)) {
				changes = TRUE;
			}
		   }
		}
	}
}




indir_calls()
{
	Cindex i;
	proc_p p;

	for (i = Cfirst(cai_set); i != (Cindex) 0; i = Cnext(i,cai_set)) {
		p = pmap[Celem(i)];  /* p does a CAI */
		Cjoin(lpi_set, &p->p_calling);
	}
	Cdeleteset(lpi_set);
	Cdeleteset(cai_set);
}



main(argc,argv)
	int argc;
	char *argv[];
{
	FILE *f, *f2, *gf2;  /* The EM input, EM output, basic block output */
	bblock_p g;
	short n, kind;
	line_p l;

	linecount = 0;
	fproc = getptable(pname); /* proc table */
	fdblock = getdtable(dname);  /* data block table */
	lpi_set = Cempty_set(plength);
	cai_set = Cempty_set(plength);
	if ((f = fopen(lname,"r")) == NULL) {
		error("cannot open %s", lname);
	}
	if ((f2 = fopen(lname2,"w")) == NULL) {
		error("cannot open %s", lname2);
	}
	if ((gf2 = fopen(bname2,"w")) == NULL) {
		error("cannot open %s",bname2);
	}
	while (getbblocks(f,&kind,&n,&g,&l)) {
		/* read EM text of one unit and
		 * (if it is a procedure)
		 * partition it into n basic blocks.
		 */
		if (kind == LDATA) {
			putunit(LDATA,(proc_p) 0,l,gf2,f2);
		} else {
			curproc->p_start = g;
			/* The global variable curproc points to the
			 * current procedure. It is set by getbblocks
			 */
			control_flow(g); /* compute pred and succ */
			dominators(g,n); /* compute immediate dominators */
			loop_detection(curproc); /* compute loops */
			interproc_analysis(curproc);
			/* Interprocedural analysis */
			cf_cleanproc(curproc);
			putunit(LTEXT,curproc,(line_p) 0,gf2,f2);
			/* output control flow graph + text */
		}
	}
	fclose(f);
	fclose(f2);
	fclose(gf2);
	indir_calls();
	trans_clos(fproc);
	/* Compute transitive closure of used/changed
	 * variables information for every procedure.
	 */
	if ((f = fopen(dname2,"w")) == NULL) {
		error("cannot open %s",dname2);
	}
	putdtable(fdblock,f);
	if ((f = fopen(pname2,"w")) == NULL) {
		error("cannot open %s",pname2);
	}
	putptable(fproc,f,TRUE);
	exit(0);
}
