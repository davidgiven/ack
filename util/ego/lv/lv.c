/* $Header$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */

/* L I V E   V A R I A B L E S   A N A L Y S I S */

#include <stdio.h>
#include <em_mnem.h>
#include <em_pseu.h>
#include <em_spec.h>
#include <em_mes.h>
#include <em_ego.h>
#include "../share/types.h"
#include "lv.h"
#include "../share/debug.h"
#include "../share/global.h"
#include "../share/lset.h"
#include "../share/cset.h"
#include "../share/def.h"
#include "../share/files.h"
#include "../share/alloc.h"
#include "../share/map.h"
#include "../share/get.h"
#include "../share/put.h"
#include "../share/aux.h"
#include "../share/init_glob.h"
#include "../share/locals.h"
#include "../share/go.h"
#include "../share/parser.h"

#define newlvbx()	(bext_p) newstruct(bext_lv)
#define oldlvbx(x)	oldstruct(bext_lv,x)


short nrglobals;
short nrvars;

STATIC int Slv;
STATIC bool mesgflag = FALSE;  /* Suppress generation of live/dead info */


STATIC clean_up()
{
	local_p *p;

	for (p = &locals[1]; p <= &locals[nrlocals]; p++) {
		oldlocal(*p);
	}
	oldmap(locals,nrlocals);
}



STATIC bool is_dir_use(l)
	line_p l;
{
	/* See if l is a direct use of some variable
	 * (i.e. not through a pointer). A LIL is a
	 * direct use of some pointer variable
	 * (and an indirect use of some other variable).
	 * A SIL is also a direct use.
	 * A LOI, however, is not an direct use of a variable.
	 * An an increment/decrement instruction is regarded
	 * as a use here, and not as a definition, as the
	 * variable is first used and than defined.
	 */

	switch(INSTR(l)) {
		case op_dee:
		case op_del:
		case op_ine:
		case op_inl:
		case op_lde:
		case op_ldl:
		case op_lil:
		case op_loe:
		case op_lol:
		case op_sil:
			return TRUE;
		default:
			return FALSE;
	}
	/* NOTREACHED */
}



STATIC bool is_indir_use(l)
	line_p l;
{
	/* See if instruction l uses some variable(s) indirectly,
	 * i.e. through a pointer or via a procedure call.
	 */

	switch(INSTR(l)) {
		case op_blm:
		case op_bls:
		case op_cai:
		case op_cal:
		case op_lar:
		case op_ldf:
		case op_lil:
		case op_lof:
		case op_loi:
		case op_los:
		case op_mon:
			return TRUE;
		default:
			return FALSE;
	}
	/* NOTREACHED */
}



STATIC bool is_def(l)
	line_p l;
{
	/* See if l does a direct definition */

	switch(INSTR(l)) {
		case op_sde:
		case op_sdl:
		case op_ste:
		case op_stl:
		case op_zre:
		case op_zrl:
			return TRUE;
		default:
			return FALSE;
	}
	/* NOTREACHED */
}


STATIC def_use(p)
	proc_p p;
{
	/* Compute DEF(b) and USE(b), for every basic block b
	 * of procedure p. DEF(b) contains the variables that
	 * are certain to be defined (assigned) in b
	 * before being used. USE(b) contains the variables
	 * that may be used in b, before being defined.
	 * (Note that uncertainty arises in the presence of
	 *  pointers and procedure calls).
	 * We compute these sets, by scanning the text of
	 * the basic block from beginning till end.
	 */

	register bblock_p b;
	register line_p l;
	short v;
	bool found;
	cset all_ind_uses;

	all_ind_uses = Cempty_set(nrvars);
	for (v = 1; v < nrlocals; v++) {
		if (!IS_REGVAR(locals[v])) {
			Cadd(LOC_TO_VARNR(v),&all_ind_uses);
		}
	}
	for (b = p->p_start; b != (bblock_p) 0; b = b->b_next) {
		USE(b) = Cempty_set(nrvars);
		DEF(b) = Cempty_set(nrvars);
		for (l = b->b_start; l != (line_p) 0; l = l->l_next) {
			if (is_def(l)) {
				/* An direct definition (i.e. not
				 * through a pointer).
				 */
				var_nr(l,&v,&found);
				if (found && !Cis_elem(v,USE(b))) {
					/* We do maintain live-dead info
					 * for this variable, and it was
					 * not used earlier in b.
					 */
					Cadd(v, &DEF(b));
				}
			} else {
				if (is_dir_use(l)) {
					var_nr(l,&v,&found);
					if (found && !Cis_elem(v,DEF(b))) {
						Cadd(v, &USE(b));
					}
				}
				if (is_indir_use(l)) {
					/* Add variable that may be used
					 * by l to USE(b).
					 */
					Cjoin(all_ind_uses,&USE(b));
				}
			}
		}
	}
	Cdeleteset(all_ind_uses);
}



STATIC unite_ins(bbset,setp)
	lset bbset;
	cset *setp;
{
	/* Take the union of L_IN(b), for all b in bbset,
	 * and put the result in setp.
	 */

	Lindex i;

	Cclear_set(setp);
	for (i = Lfirst(bbset); i != (Lindex) 0; i = Lnext(i,bbset)) {
		Cjoin(L_IN((bblock_p) Lelem(i)), setp);
	}
}



STATIC solve_lv(p)
	proc_p p;
{
	/* Solve the data flow equations for Live Variables,
	 * for procedure p. These equations are:
	 *  (1)   IN[b] = OUT[b] - DEF[b] + USE[b]
	 *  (2)   OUT(b) = IN(s1) + ... + IN(sn) ;
	 *        where SUCC(b) = {s1, ... , sn}
	 */

	register bblock_p b;
	cset newout = Cempty_set(nrvars);
	bool change = TRUE;

	for (b = p->p_start; b != (bblock_p) 0; b = b->b_next) {
		L_IN(b) = Cempty_set(nrvars);
		Ccopy_set(USE(b), &L_IN(b));
		L_OUT(b) = Cempty_set(nrvars);
	}
	while (change) {
		change = FALSE;
		for (b = p->p_start; b != (bblock_p) 0; b = b->b_next) {
			unite_ins(b->b_succ,&newout);
			if (!Cequal(newout,L_OUT(b))) {
				change = TRUE;
				Ccopy_set(newout, &L_OUT(b));
				Ccopy_set(newout, &L_IN(b));
				Csubtract(DEF(b), &L_IN(b));
				Cjoin(USE(b), &L_IN(b));
			}
		}
	}
	Cdeleteset(newout);
}


STATIC live_variables_analysis(p)
	proc_p p;
{
	make_localtab(p);
	nrvars = nrglobals + nrlocals;
	def_use(p);
	solve_lv(p);
}


STATIC init_live_dead(b)
	bblock_p b;
{
	/* For every register variable, see if it is
	 * live or dead at the end of b.
	 */

	register short v;
	local_p loc;

	for (v = 1; v <= nrlocals; v++) {
		loc = locals[v];
		if (IS_REGVAR(loc) && Cis_elem(LOC_TO_VARNR(v),L_OUT(b))) {
			LIVE(loc);
		} else {
			DEAD(loc);
		}
	}
}



STATIC line_p make_mesg(mesg,loc)
	short mesg;
	local_p loc;
{
	/* Create a line for a message stating that
	 * local variable loc is live/dead. This message
	 * looks like: "mes ms_ego,ego_live,off,size" or
	 * "mes ms_ego,ego_dead,off,size".
	 */

	line_p l = newline(OPLIST);
	register arg_p ap;

	l->l_instr = ps_mes;
	ap = ARG(l) = newarg(ARGOFF);
	ap->a_a.a_offset = ms_ego;
	ap = ap->a_next = newarg(ARGOFF);
	ap->a_a.a_offset = mesg;
	ap = ap->a_next = newarg(ARGOFF);
	ap->a_a.a_offset = loc->lc_off;
	ap = ap->a_next = newarg(ARGOFF);
	ap->a_a.a_offset = loc->lc_size;
	return l;
}



STATIC block_entry(b,prev)
	bblock_p b,prev;
{
	short v,vn;
	local_p loc;
	bool was_live, is_live;

	/* Generate a live/dead message for every register variable that
	 * was live at the end of prev, but dead at the beginning of b,
	 * or v.v. If prev = 0 (i.e. begin of procedure), parameters were
	 * live, normal local variables were dead.
	 */

	for (v = 1; v <= nrlocals; v++) {
		loc = locals[v];
		if (IS_REGVAR(loc)) {
			vn = LOC_TO_VARNR(v);
			if (prev == (bblock_p) 0) {
				was_live = loc->lc_off >= 0;
			} else {
				was_live = Cis_elem(vn,L_OUT(prev));
			}
			is_live = Cis_elem(vn,L_IN(b));
			if (was_live != is_live) {
				app_block(make_mesg((is_live?ego_live:ego_dead),loc),b);
			}
		}
	}
}



STATIC app_block(l,b)
	line_p l;
	bblock_p b;
{
	line_p x = b->b_start;

	if (x != (line_p) 0 && INSTR(x) == ps_pro) {
		/* start of procedure; append after pro pseudo ! */
		if ((l->l_next = x->l_next) != (line_p) 0) {
			PREV(l->l_next) = l;
		}
		x->l_next = l;
		PREV(l) = x;
	} else {
		if ((l->l_next = x) != (line_p) 0) {
			PREV(l->l_next) = l;
		}
		b->b_start = l;
		PREV(l) = (line_p) 0;
	}
}



STATIC definition(l,useless_out,v_out,mesgflag)
	line_p l;
	bool *useless_out;
	short *v_out;
	bool mesgflag;
{
	/* Process a definition. If the defined (register-) variable
	 * is live after 'l', then create a live-message and put
	 * it after 'l'.
	 */

	short v;
	bool found;
	local_p loc;

	*useless_out = FALSE;
	var_nr(l,&v,&found);
	if (found && IS_LOCAL(v)) {
		*v_out = v;
		loc = locals[TO_LOCAL(v)];
		if (IS_REGVAR(loc)) {
			/*	Tricky stuff here. Make sure that a variable
				that is assigned to is alive, at least for
				a very very short time. Otherwize, the
				register allocation pass might think that it
				is never alive, and (incorrectly) use the
				same register for this variable as for 
				another variable, that is alive at this point.
				If this variable is dead after the assignment,
				the two messages (ego_live, ego_dead) are right
				after each other. Luckily, this IS an interval.
			*/
			if (!mesgflag) {
				appnd_line(make_mesg(ego_live,loc), l);
				l = l->l_next;
			}
			if (IS_LIVE(loc)) {
				DEAD(loc);
			} else {
				if (!mesgflag) {
					appnd_line(make_mesg(ego_dead, loc), l);
				}
				*useless_out = TRUE;
			}
		}
	}
}




STATIC use(l,mesgflag)
	line_p l;
	bool mesgflag;
{
	/* Process a use. If the defined (register-) variable
	 * is dead after 'l', then create a dead-message and put
	 * it after 'l'.
	 */

	short v;
	bool found;
	local_p loc;

	var_nr(l,&v,&found);
	if (found && IS_LOCAL(v)) {
		loc = locals[TO_LOCAL(v)];
		if (IS_REGVAR(loc) && IS_DEAD(loc)) {
			if (!mesgflag) {
				appnd_line(make_mesg(ego_dead,loc), l);
			}
			LIVE(loc);
		}
	}
}



STATIC nothing() { }  /* No action to be undertaken at level 0 of parser */

STATIC rem_code(l1,l2,b)
	line_p l1,l2;
	bblock_p b;
{
	line_p l,x,y,next;

	x = PREV(l1);
	y = l2->l_next;
	for (l = l1; l != l2; l = next) {
		next = l->l_next;
		oldline(l);
	}
	if (x == (line_p) 0) {
		b->b_start = y;
	} else {
		x->l_next = y;
	}
	if (y != (line_p) 0) {
		PREV(y) = x;
	}
}




#define SIZE(v)	((offset) locals[TO_LOCAL(v)]->lc_size)




lv_mesg(p,mesgflag)
	proc_p p;
	bool mesgflag;
{
	/* Create live/dead messages for every possible register
	 * variable of p. A dead-message is put after a "use" of
	 * such a variable, if the variable becomes dead just
	 * after the use (i.e. this was its last use).
	 * A live message is put after a "definition" of such
	 * a variable, if the variable becomes live just
	 * after the definition (which will usually be the case).
	 * We traverse every basic block b of p from the last
	 * instruction of b backwards to the beginning of b.
	 * Initially, all variables that are dead at the end
	 * of b are marked dead. All others are marked live.
	 * If we come accross a definition of a variable X that
	 * was marked live, we put a live-message after the
	 * definition and mark X dead.
	 * If we come accross a use of a variable X that
	 * was marked dead, we put a dead-message after the
	 * use and mark X live.
	 * So at any point, the mark of X tells whether X is
	 * live or dead immediately before (!) that point.
	 * We also generate a message at the start of a basic block
	 * for every variable that was live at the end of the (textually)
	 * previous block, but dead at the entry of this block, or v.v.
	 * On the fly, useless assignments are removed.
	 */

	register bblock_p b;
	register line_p l;
	line_p lnp, prev;
	bblock_p prevb = (bblock_p) 0;
	short v;
	bool useless;

	for (b = p->p_start; b != (bblock_p) 0; b = b->b_next) {
		block_entry(b,prevb); /* generate message at head of block */
		prevb = b;
		if (!mesgflag) {
			init_live_dead(b);
		}
		for (l = last_instr(b); l != (line_p) 0; l = prev) {
			/* traverse backwards! */
			prev = PREV(l);
			if (is_def(l)) {
				definition(l,&useless,&v,mesgflag);
				if (useless &&   /* assignment to dead var. */
				    parse(prev,SIZE(v),&lnp,0,nothing)) {
					/* The code "VAR := expression" can
					 * be removed. 'l' is the "STL VAR",
					 * lnp is the beginning of the EM code
					 * for the expression.
					 */
					prev = PREV(lnp);
					rem_code(lnp,l,b);
OUTVERBOSE("useless assignment ,proc %d,local %d", curproc->p_id,
  (int) locals[TO_LOCAL(v)]->lc_off);
					Slv++;
				}
				else {
				}
			} else {
				if (is_dir_use(l))  {
					use(l,mesgflag);
				}
			}
		}
	}
}


STATIC lv_extend(p)
	proc_p p;
{
	/* Allocate extended data structures for Use Definition analysis */

	register bblock_p b;

	for (b = p->p_start; b != (bblock_p) 0; b = b->b_next) {
		b->b_extend = newlvbx();
	}
}


STATIC lv_cleanup(p)
	proc_p p;
{
	/* Deallocate extended data structures for Use Definition analysis */

	register bblock_p b;

	for (b = p->p_start; b != (bblock_p) 0; b = b->b_next) {
		Cdeleteset(USE(b));
		Cdeleteset(DEF(b));
		Cdeleteset(L_IN(b));
		Cdeleteset(L_OUT(b));
		oldlvbx(b->b_extend);
	}
}

lv_flags(p)
	char *p;
{
	switch(*p) {
		case 'N':
			mesgflag = TRUE;
			break;
	}
}


lv_optimize(p)
	proc_p p;
{
	if (IS_ENTERED_WITH_GTO(p)) return;
	locals = (local_p *) 0;
	lv_extend(p);
	live_variables_analysis(p);
	lv_mesg(p,mesgflag);
	/* generate live-dead messages for regvars */
	lv_cleanup(p);
	clean_up();
}



main(argc,argv)
	int argc;
	char *argv[];
{
	go(argc,argv,init_globals,lv_optimize,no_action,lv_flags);
	report("useless assignments deleted",Slv);
	exit(0);
}
