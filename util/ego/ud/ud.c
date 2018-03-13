/* $Id$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/*  U S E  -  D E F I N I T I O N   A N A L Y S I S */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <em_spec.h>
#include "../share/types.h"
#include "ud.h"
#include "../share/debug.h"
#include "../share/global.h"
#include "../share/lset.h"
#include "../share/cset.h"
#include "../share/def.h"
#include "../share/files.h"
#include "../share/map.h"
#include "../share/get.h"
#include "../share/put.h"
#include "../share/alloc.h"
#include "../share/aux.h"
#include "../share/init_glob.h"
#include "../share/locals.h"
#include "../share/go.h"
#include "ud_defs.h"
#include "ud_const.h"
#include "ud_copy.h"

/* core allocation macros */
#define newudbx()	(bext_p) newstruct(bext_ud)
#define oldudbx(x)	oldstruct(bext_ud,x)

short nrglobals;
short nrvars;

int Svalue,Svariable;

cond_p globl_cond_tab,local_cond_tab;

STATIC cond_p getcondtab(f)
	FILE *f;
{
	int l,i;
	cond_p tab;

	fscanf(f,"%d",&l);
	tab = newcondtab(l);
	for (i = 0; i < l; i++) {
		fscanf(f,"%hd %hd %hd",&tab[i].mc_cond,&tab[i].mc_tval,
			 &tab[i].mc_sval);
	}
	assert(tab[l-1].mc_cond == DEFAULT);
	return tab;
}


STATIC void ud_machinit(void *vp)
{
	FILE *f = vp;
	char s[100];

	for (;;) {
		while(getc(f) != '\n');
		fscanf(f,"%s",s);
		if (strcmp(s,"%%UD") == 0)break;
	}
	globl_cond_tab = getcondtab(f);
	local_cond_tab = getcondtab(f);
}



STATIC bool test_cond(cond,val)
	short cond;
	offset val;
{
	switch(cond) {
		case DEFAULT:
			return TRUE;
		case FITBYTE:
			return val >= -128 && val < 128;
	}
	assert(FALSE);
	/* NOTREACHED */
}


STATIC short map_value(tab,val,time)
	struct cond_tab tab[];
	offset val;
	bool time;
{
	cond_p p;

	for (p = &tab[0]; ; p++) {
		if (test_cond(p->mc_cond,val)) {
			return (time ? p->mc_tval : p->mc_sval);
		}
	}
}


STATIC init_root(root)
	bblock_p root;
{
	/* Initialise the IN OUT sets of the entry block of the
	 * current procedure. Global variables and parameters
	 * already have a value at this point, although we do
	 * not know which value. Therefor, implicit definitions
	 * to all global variables and parameters are
	 * put in IN.
	 */

	short v;

	for (v = 1; v <= nrglobals; v++) {
		Cadd(IMPLICIT_DEF(GLOB_TO_VARNR(v)), &IN(root));
	}
	for (v = 1; v <= nrlocals; v++) {
		if (locals[v]->lc_off >= 0) {
			Cadd(IMPLICIT_DEF(LOC_TO_VARNR(v)),&IN(root));
		}
	}
	/* OUT(root) = IN(root) - KILL(root) + GEN(root) */
	Ccopy_set(IN(root),&OUT(root));
	Csubtract(KILL(root),&OUT(root));
	Cjoin(GEN(root),&OUT(root));
}




STATIC unite_outs(bbset,setp)
	lset bbset;
	cset *setp;
{
	/* Take the union of OUT(b), for all b in bbset,
	 * and put the result in setp.
	 */

	Lindex i;

	Cclear_set(setp);
	for (i = Lfirst(bbset); i != (Lindex) 0; i = Lnext(i,bbset)) {
		Cjoin(OUT((bblock_p) Lelem(i)), setp);
	}
}



STATIC solve_equations(p)
	proc_p p;
{
	/* Solve the data flow equations for reaching
	 * definitions of procedure p.
	 * These equations are:
	 *  (1)  OUT(b) = IN(b) - KILL(b) + GEN(b)
	 *  (2)  IN(b)  = OUT(p1) + .. + OUT(pn) ; 
	 *       where PRED(b) = {p1, .. , pn}
	 * We use the iterative algorithm of Aho&Ullman to
	 * solve the equations.
	 */

	register bblock_p b;
	bool     change;
	cset     newin;

	/* initializations */
	newin = Cempty_set(nrdefs);
	for (b = p->p_start; b != (bblock_p) 0; b = b->b_next) {
		IN(b) = Cempty_set(nrdefs);
		OUT(b) = Cempty_set(nrdefs);
		Ccopy_set(GEN(b), &OUT(b));
	}
	init_root(p->p_start);
	/* Global variables and parameters have already a value
	 * at the procedure entry block.
	 */
	change = TRUE;
	/* main loop */
	while (change) {
		change = FALSE;
		for (b = p->p_start->b_next; b != (bblock_p) 0; b = b->b_next) {
			unite_outs(b->b_pred, &newin);
			/* newin = OUT(p1) + .. + OUT(pn) */
			if (!Cequal(newin,IN(b))) {
				change = TRUE;
				Ccopy_set(newin, &IN(b));
				Ccopy_set(IN(b),   &OUT(b));
				Csubtract(KILL(b), &OUT(b));
				Cjoin(GEN(b),      &OUT(b));
			}
		}
	}
	for (b = p->p_start; b != (bblock_p) 0; b = b->b_next) {
		Cdeleteset(KILL(b));
		Cdeleteset(OUT(b));
	}
	Cdeleteset(newin);
}



short global_addr_cost()
{
	return add_timespace(map_value(globl_cond_tab,(offset) 0,TRUE),
			     map_value(globl_cond_tab,(offset) 0,FALSE));
}

short local_addr_cost(off)
	offset off;
{
	return add_timespace(map_value(local_cond_tab,off,TRUE),
			     map_value(local_cond_tab,off,FALSE));
}



STATIC bool fold_is_desirable(old,new)
	line_p old,new;
{
	/* See if it is desirable to replace the variable used by the
	 * EM instruction 'old' by the variable used by 'new'.
	 * We do not replace 'cheaply addressable variables' by 'expensively
	 * addressable variables'. E.g. if we're optimizing object code size,
	 * we do not replace a local variable by a global variable on a VAX,
	 * because the former occupies 1 or 2 bytes and the latter occupies
	 * 4 bytes.
	 * If 2 local variables are equally expensive to address, we replace
	 * the first one by the second only if the first one is used at
	 * least as many times as the second one.
	 */

	local_p oldloc,newloc;
	short old_cost,new_cost,nr;
	bool ok;

	if (TYPE(old) == OPOBJECT) {
		/* old variable is a global variable */
		return TYPE(new) != OPOBJECT && 
		       global_addr_cost() >=
		       local_addr_cost(off_set(new));
	}
	find_local(off_set(old),&nr,&ok);
	assert(ok);
	oldloc = locals[nr];
	old_cost = local_addr_cost(off_set(old));
	if (TYPE(new) == OPOBJECT) {
		return oldloc->lc_score == 2 || /* old var. can be eliminated */
		       old_cost > global_addr_cost();
	}
	find_local(off_set(new),&nr,&ok);
	assert(ok);
	newloc = locals[nr];
	new_cost = local_addr_cost(off_set(new));
	return old_cost > new_cost ||
	       (old_cost == new_cost && oldloc->lc_score < newloc->lc_score);
}



#ifdef TRACE
/*********** TRACING ROUTINES ***********/

pr_localtab() {
	short i;
	local_p lc;

	fprintf(stderr,"LOCAL-TABLE (%d)\n\n",nrlocals);
	for (i = 1; i <= nrlocals; i++) {
		lc = locals[i];
		fprintf(stderr,"LOCAL %d\n",i);
		fprintf(stderr,"\toffset= %ld\n",lc->lc_off);
		fprintf(stderr,"\tsize=   %d\n",lc->lc_size);
		fprintf(stderr,"\tflags=  %d\n",lc->lc_flags);
	}
}

pr_globals()
{
	dblock_p d;
	obj_p obj;

	fprintf(stderr,"GLOBALS (%d)\n\n",nrglobals);
	fprintf(stderr,"ID\tGLOBNR\n");
	for (d = fdblock; d != (dblock_p) 0; d = d->d_next) {
		for (obj = d->d_objlist; obj != (obj_p) 0; obj = obj->o_next) {
			if (obj->o_globnr != 0) {
				fprintf(stderr,"%d\t%d\n",
				    obj->o_id,obj->o_globnr);
			}
		}
	}
}

extern char em_mnem[];

pr_defs()
{
	short i;
	line_p l;

	fprintf(stderr,"DEF TABLE\n\n");
	for (i = 1; i <= nrexpldefs; i++) {
		l = defs[i];
		fprintf(stderr,"%d\t%s ",EXPL_TO_DEFNR(i),
			&em_mnem[(INSTR(l)-sp_fmnem)*4]);
		switch(TYPE(l)) {
			case OPSHORT:
				fprintf(stderr,"%d\n",SHORT(l));
				break;
			case OPOFFSET:
				fprintf(stderr,"%ld\n",OFFSET(l));
				break;
			case OPOBJECT:
				fprintf(stderr,"%d\n",OBJ(l)->o_id);
				break;
			default:
				assert(FALSE);
		}
	}
}


pr_set(name,k,s,n)
	char *name;
	cset s;
	short k,n;
{
	short i;

	fprintf(stderr,"%s(%d) =\t{",name,k);
	for (i = 1; i <= n; i++) {
		if (Cis_elem(i,s)) {
			fprintf(stderr,"%d ",i);
		}
	}
	fprintf(stderr,"}\n");
}

pr_blocks(p)
	proc_p p;
{
	bblock_p b;
	short n;

	for (b = p->p_start; b != 0; b = b->b_next) {
		fprintf(stderr,"\n");
		n = b->b_id;
		pr_set("GEN",n,GEN(b),nrdefs);
		pr_set("KILL",n,KILL(b),nrdefs);
		pr_set("IN ",n,IN(b),nrdefs);
		pr_set("OUT",n,OUT(b),nrdefs);
		pr_set("CHGVARS",n,CHGVARS(b),nrvars);
	}
}

pr_copies()
{
	short i;

	fprintf(stderr,"\nCOPY TABLE\n\n");
	for (i = 1; i <= nrdefs; i++) {
		if (def_to_copynr[i] != 0) {
			fprintf(stderr,"%d\t%d\n",i,def_to_copynr[i]);
		}
	}
}

pr_cblocks(p)
	proc_p p;
{
	bblock_p b;
	short n;

	for (b = p->p_start; b != 0; b = b->b_next) {
		fprintf(stderr,"\n");
		n = b->b_id;
		pr_set("CGEN",n,C_GEN(b),nrcopies);
		pr_set("CKILL",n,C_KILL(b),nrcopies);
		pr_set("CIN ",n,C_IN(b),nrcopies);
		pr_set("COUT",n,C_OUT(b),nrcopies);
	}
}

/*********** END TRACING ********/

#endif

STATIC ud_analysis(p)
	proc_p p;
{
	/* Perform use-definition analysis on procedure p */

	make_localtab(p);  /* See for which local we'll keep ud-info */
#ifdef TRACE
	pr_localtab();
#endif
	nrvars = nrglobals + nrlocals;
	make_defs(p);  /* Make a table of all useful definitions in p */
#ifdef TRACE
	pr_defs();
#endif
	nrdefs = nrexpldefs + nrvars; /* number of definitions */
	gen_sets(p); /* compute GEN(b), for every basic block b */
	kill_sets(p); /* compute KILL(b), for every basic block b */
	solve_equations(p); /* solve data flow equations for p */
#ifdef TRACE
	pr_blocks(p);
#endif
}



STATIC clean_maps()
{
	local_p *p;
	cset *v;

	oldmap(defs,nrexpldefs);
	for (p = &locals[1]; p <= &locals[nrlocals]; p++) {
		oldlocal(*p);
	}
	oldmap(locals,nrlocals);
	for (v = &vardefs[1]; v <= &vardefs[nrvars]; v++) {
		Cdeleteset(*v);
	}
	oldmap(vardefs,nrvars);
}



STATIC bool try_optim(l,b)
	line_p l;
	bblock_p b;
{
	/* Try copy propagation and constant propagation */

	line_p def;
	offset val;
	short defnr;


	if (is_use(l) && (def = unique_def(l,b,&defnr)) != (line_p) 0) {
		if (is_copy(def)) {
			if (value_retained(def,defnr,l,b) &&
			    fold_is_desirable(l,PREV(def))) {
				fold_var(l,PREV(def),b);
				OUTVERBOSE("vp:variable folded in proc %d",
					    curproc->p_id,0);
				Svariable++;
				return TRUE;
			}
		} else {
			if (value_known(def,&val)) {
				fold_const(l,b,val);
				OUTVERBOSE("vp:value folded in proc %d",
				   curproc->p_id,0);
				Svalue++;
				return TRUE;
			}
		}
	}
	return FALSE;
}



value_propagation(p)
	proc_p p;
{
	/* Apply value propagation to procedure p */

	bool	changes;
	bblock_p b;
	line_p	l, next;

	changes = TRUE;
	/* If a statement like A := B is folded to A := constant,
	 * new opportunities for constant folding may arise,
	 * e.g. the value of A might be statically known too now.
	 */

	 while (changes) {
		changes = FALSE;
		for (b = p->p_start; b != (bblock_p) 0; b = b->b_next) {
			for (l = b->b_start; l != (line_p) 0; l = next) {
				next = l->l_next;
				if (try_optim(l,b)) {
					changes = TRUE;
				}
			}
		}
	}
	oldmap(copies,nrcopies);
	oldtable(def_to_copynr,nrdefs);
}


STATIC ud_extend(p)
	proc_p p;
{
	/* Allocate extended data structures for Use Definition analysis */

	register bblock_p b;

	for (b = p->p_start; b != (bblock_p) 0; b = b->b_next) {
		b->b_extend = newudbx();
	}
}


STATIC ud_cleanup(p)
	proc_p p;
{
	/* Deallocate extended data structures for Use Definition analysis */

	register bblock_p b;

	for (b = p->p_start; b != (bblock_p) 0; b = b->b_next) {
		Cdeleteset(GEN(b));
		Cdeleteset(IN(b));
		Cdeleteset(C_GEN(b));
		Cdeleteset(C_KILL(b));
		Cdeleteset(C_IN(b));
		Cdeleteset(C_OUT(b));
		Cdeleteset(CHGVARS(b));
		oldudbx(b->b_extend);
	}
}


void ud_optimize(void *vp)
{
	proc_p p = vp;

	if (IS_ENTERED_WITH_GTO(p)) return;
	ud_extend(p);
	locals = (local_p *) 0;
	vardefs = (cset *) 0;
	defs = (line_p *) 0;
	ud_analysis(p);
	copy_analysis(p);
#ifdef TRACE
	pr_copies();
	pr_cblocks(p);
#endif
	value_propagation(p);
	ud_cleanup(p);
	clean_maps();
}

main(argc,argv)
	int argc;
	char *argv[];
{
	go(argc,argv,init_globals,ud_optimize,ud_machinit,no_action);
	report("values folded",Svalue);
	report("variables folded",Svariable);
	exit(0);
}



