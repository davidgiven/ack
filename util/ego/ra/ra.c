/* $Id$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/*
 *   R E G I S T E R    A L L O C A T I O N
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <em_reg.h>
#include "../share/types.h"
#include "../share/debug.h"
#include "../share/global.h"
#include "../share/files.h"
#include "../share/get.h"
#include "../share/put.h"
#include "../share/lset.h"
#include "../share/map.h"
#include "../share/alloc.h"
#include "../share/go.h"
#include "ra.h"
#include "ra_items.h"
#include "ra_allocl.h"
#include "ra_profits.h"
#include "ra_pack.h"
#include "ra_xform.h"


#define newrabx()	(bext_p)	newstruct(bext_ra)
#define newralpx()	(lpext_p)	newstruct(lpext_ra)
#define oldrabx(x)	oldstruct(bext_ra,x)
#define oldralpx(x)	oldstruct(lpext_ra,x)

short alloc_id;
static item_p items[NRITEMTYPES];
int nrinstrs;
line_p *instrmap;

cond_p alocaltab[NRREGTYPES][NRREGTYPES],alocaddrtab[NRREGTYPES][NRREGTYPES],
	aconsttab,adconsttab,aglobaltab,aproctab;
cond_p olocaltab[NRREGTYPES],olocaddrtab[NRREGTYPES],
	oconsttab,odconsttab,oglobaltab,oproctab;
cond_p regsav_cost;

short regs_available[] = {
	/* Actually machine dependent; this is for vax2 */
	3,	/* reg_any i.e. data regs */
	0,	/* reg_loop */
	3,	/* reg_pointer i.e. address reg. */
	0	/* reg_float */
} ;

short use_any_as_pointer = 0;

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

get_atab(f,tab)
	FILE *f;
	cond_p tab[NRREGTYPES][NRREGTYPES];
{
	int i,cnt,totyp,regtyp;
	
	fscanf(f,"%d",&cnt);
	for (i = 0; i < cnt; i++) {
		fscanf(f,"%d %d",&regtyp,&totyp);
		assert(regtyp >= 0  && regtyp < NRREGTYPES);
		assert(totyp >= 0  && totyp < NRREGTYPES);
		tab[regtyp][totyp] = getcondtab(f);
	}
}


get_otab(f,tab)
	FILE *f;
	cond_p tab[NRREGTYPES];
{
	int i,cnt,regtyp;
	
	fscanf(f,"%d",&cnt);
	for (i = 0; i < cnt; i++) {
		fscanf(f,"%d",&regtyp);
		assert(regtyp >= 0  && regtyp < NRREGTYPES);
		tab[regtyp] = getcondtab(f);
	}
}



STATIC void ra_machinit(void *vp)
{
	/* Read target machine dependent information for this phase */
	FILE *f = vp;
	char s[100];

	for (;;) {
		while(getc(f) != '\n');
		fscanf(f,"%99s",s);
		if (strcmp(s,"%%RA") == 0)break;
	}
	fscanf(f,"%hd",&regs_available[reg_any]);
	fscanf(f,"%hd",&regs_available[reg_pointer]);
	fscanf(f,"%hd",&regs_available[reg_float]);
	fscanf(f,"%hd",&use_any_as_pointer);
	get_atab(f,alocaltab);
	get_atab(f,alocaddrtab);
	aconsttab = getcondtab(f);
	adconsttab = getcondtab(f);
	aglobaltab = getcondtab(f);
	aproctab = getcondtab(f);
	get_otab(f,olocaltab);
	get_otab(f,olocaddrtab);
	oconsttab = getcondtab(f);
	odconsttab = getcondtab(f);
	oglobaltab = getcondtab(f);
	oproctab = getcondtab(f);
	regsav_cost = getcondtab(f);
}


STATIC bblock_p header(lp)
	loop_p lp;
{
	/* Try to determine the 'header' block of loop lp.
	 * If 'e' is the entry block of loop L, then block 'b' is
	 * called the header block of L, iff:
	 *	SUCC(b) = {e} & PRED(e) = {b}
	 * If lp has no header block, 0 is returned.
	 */

	bblock_p x = lp->lp_entry->b_idom;

	if (x != (bblock_p) 0 && Lnrelems(x->b_succ) == 1 &&
	    (bblock_p) Lelem(Lfirst(x->b_succ)) == lp->lp_entry) {
		return x;
	}
	return (bblock_p) 0;
}


STATIC ra_extproc(p)
	proc_p p;
{
	/* Allocate the extended data structures for procedure p */

	register loop_p lp;
	register Lindex pi;
	register bblock_p b;

	for (pi = Lfirst(p->p_loops); pi != (Lindex) 0;
	   pi = Lnext(pi,p->p_loops)) {
		lp = (loop_p) Lelem(pi);
		lp->lp_extend = newralpx();
		lp->LP_HEADER = header(lp);
	}
	for (b = p->p_start; b != (bblock_p) 0; b = b->b_next) {
		b->b_extend = newrabx();
	}
}




STATIC ra_cleanproc(p)
	proc_p p;
{
	/* Allocate the extended data structures for procedure p */

	register loop_p lp;
	register Lindex pi;
	register bblock_p b;

	for (pi = Lfirst(p->p_loops); pi != (Lindex) 0;
	   pi = Lnext(pi,p->p_loops)) {
		lp = (loop_p) Lelem(pi);
		oldralpx(lp->lp_extend);
	}
	for (b = p->p_start; b != (bblock_p) 0; b = b->b_next) {
		oldrabx(b->b_extend);
	}
}



STATIC loop_blocks(p)
	proc_p p;
{
	/* Compute the LP_BLOCKS sets for all loops of p */

	register bblock_p b;
	register Lindex i;

	for (b = p->p_start; b != (bblock_p) 0; b = b->b_next) {
		for (i = Lfirst(b->b_loops); i != (Lindex) 0;
		   i = Lnext(i,b->b_loops)) {
			Ladd(b,&(((loop_p) Lelem(i))->LP_BLOCKS));
		}
	}
}




STATIC make_instrmap(p,map)
	proc_p p;
	line_p map[];
{
	/* make the instructions map of procedure p */

	register bblock_p b;
	register line_p l;
	register int i = 0;

	for (b = p->p_start; b != (bblock_p) 0; b = b->b_next) {
		b->B_BEGIN = i; /* number of first instruction */
		for (l = b->b_start; l != (line_p) 0; l = l->l_next) {
			map[i++] = l;
		}
		b->B_END = i-1; /* number of last instruction */
	}
}



STATIC bool useful_item(item)
	item_p item;
{
	/* See if it may be useful to put the item in a register.
	 * A local variable may always be put in a register.
	 * Other items must be used at least twice.
	 */

	int nruses = Lnrelems(item->it_usage);	
	assert (nruses > 0); /* otherwise it would not be an item! */
	return nruses > 1 || item->it_type == LOCALVAR;
}


STATIC cleantimeset(s)
	lset s;
{
	register Lindex i;
	register time_p t;

	for (i = Lfirst(s); i != (Lindex) 0; i = Lnext(i,s)) {
		t = (time_p) Lelem(i);
		oldtime(t);
	}
	Ldeleteset(s);
}



STATIC item_p cat_items(items)
	item_p items[];
{
	/* Make one item list out of an array of itemlists.
	 * Remove items that are used only once.
	 */

	register item_p it;
	item_p *ip,head,next;
	int t;


	ip = &head;
	for (t = 0; t < NRITEMTYPES;t++) {
		for ( it = items[t]; it != (item_p) 0; it = next) {
			next = it->it_next;
			if (!it->it_desirable || !useful_item(it)) {
				cleantimeset(it->it_usage);
				olditem(it);
			} else {
				*ip = it;
				ip = &it->it_next;
			}
		}
	}
	*ip = (item_p) 0;
	return head;
}




STATIC clean_interval(list)
	interv_p list;
{
	register interv_p x,next;

	for (x = list; x != (interv_p) 0; x = next) {
		next = x->i_next;
		oldinterval(x);
	}
}



STATIC clean_allocs(list)
	alloc_p list;
{
	register alloc_p x,next;

	for (x = list; x != (alloc_p) 0; x = next) {
		next = x->al_next;
		clean_interval(x->al_timespan);
		Cdeleteset(x->al_rivals);
		Ldeleteset(x->al_inits);
		clean_interval(x->al_busy);
		clean_allocs(x->al_mates);
		oldalloc(x);
	}
}



STATIC cleanitems(list)
	item_p list;
{
	register item_p x,next;

	for (x = list; x != (item_p) 0; x = next ) {
		next = x->it_next;
		cleantimeset(x->it_usage);
		olditem(x);
	}
}


/* ARGSUSED */
void ra_initialize(void *null)
{
	init_replacements(ps,ws);
}


void ra_optimize(void *vp)
{
	proc_p p = vp;
	item_p itemlist;
	alloc_p alloclist,packed,unpacked;
	offset locls;
	bool time_opt = (time_space_ratio == 100);

	if (IS_ENTERED_WITH_GTO(p)) return;
	ra_extproc(p);
	loop_blocks(p);
	alloc_id =0;
	locls = p->p_localbytes;
	build_itemlist(p,items,&nrinstrs);
	instrmap = (line_p *) newmap(nrinstrs-1); /* map starts counting at 0 */
	make_instrmap(p,instrmap);
	build_lifetimes(items);
	/* print_items(items,p); */
	/* statistics(items); */
	itemlist = cat_items(items); /* make one list */
	alloclist = build_alloc_list(p,Lnrelems(p->p_loops),
				     itemlist);
	build_rivals_graph(alloclist);
	compute_profits(alloclist,time_opt);
	/* print_allocs(alloclist); */
	pack(alloclist,time_opt,&packed,&unpacked,p);
	stat_regusage(packed);
	xform_proc(p,packed,nrinstrs,instrmap);
	/* print_allocs(packed); */
	p->p_localbytes = locls;
	/* don't really allocate dummy local variables! */
	rem_locals(p,packed); 
	rem_formals(p,packed); 
	/* remove storage for real locals that
	 *are always put in register .
	 */
	clean_allocs(unpacked);
	clean_allocs(packed);
	cleanitems(itemlist);
	oldmap(instrmap,nrinstrs-1);
	ra_cleanproc(p);
}



main(argc,argv)
	int argc;
	char *argv[];
{
	go(argc,argv,ra_initialize,ra_optimize,ra_machinit,no_action);
	exit(0);
}


/***************************************************************************/
/***************************************************************************/
/***************************************************************************/

/* debugging stuff */



char *str_types[] = {
	"local variable",
	"addr. of local",
	"addr. of external",
	"addr. of procedure",
	"constant",
	"double constant"
};

char *str_regtypes[] = {
	"any",
	"loop",
	"pointer",
	"float"
};


print_items(items,p)
	item_p items[];
	proc_p p;
{
	int t;
	item_p item;
	interv_p iv;

	fprintf(stderr, "BEGIN PROCEDURE %d\n",p->p_id);
	for (t = 0; t < NRITEMTYPES;t++) {
		for (item = items[t]; item != (item_p) 0;item = item->it_next) {
			fprintf(stderr, "\nitemtype = %s\n",str_types[t]);
			if (t == GLOBL_ADDR) {
				fprintf(stderr, "id of external = %d\n",
					item->i_t.it_obj->o_id);
			} else {
				fprintf(stderr, "offset = %ld\n",
					item->i_t.it_off);
			}
			fprintf(stderr, "regtype = %s\n",str_regtypes[item->it_regtype]);
			fprintf(stderr, "size = %d\n",item->it_size);
			fprintf(stderr, "#usages = %d\n", Lnrelems(item->it_usage));
			fprintf(stderr, "lifetime = {");
			for (iv = item->it_lives; iv != (interv_p) 0;
			     iv = iv->i_next) {
				fprintf(stderr, "(%d,%d) ",iv->i_start,iv->i_stop);
			}
			fprintf(stderr, "} \n");
		}
	}
	fprintf(stderr, "END PROCEDURE %d\n\n",p->p_id);
}


print_allocs(list)
	alloc_p list;
{
	alloc_p al,m;
	item_p item;
	short t;
	interv_p iv;

	fprintf(stderr,"BEGIN ALLOCLIST of proc %d\n",curproc->p_id);
	for (m = list ; m != (alloc_p) 0; m = m->al_next) {
		for (al = m; al != (alloc_p) 0; al = al->al_mates) {
			item = al->al_item;
			t = item->it_type;
			fprintf(stderr,"\nitem: [type = %s, ",str_types[t]);
			switch(t) {
			case GLOBL_ADDR:
				fprintf(stderr,"id = %d]\n", item->i_t.it_obj->o_id);
				break;
			case PROC_ADDR:
				fprintf(stderr,"id = %d]\n", item->i_t.it_proc->p_id);
				break;
			default:
				fprintf(stderr,"offset = %ld]\n", item->i_t.it_off);
			}
			fprintf(stderr,"#usages(static) = %d\n",al->al_susecount);
			fprintf(stderr,"#usages(dyn) = %d\n",al->al_dusecount);
			fprintf(stderr,"#inits = %d\n",Lnrelems(al->al_inits));
			fprintf(stderr,"isloop = %d\n",al->al_isloop);
			fprintf(stderr,"timespan = {");
			for (iv = al->al_timespan; iv != (interv_p) 0;
			     iv = iv->i_next) {
				fprintf(stderr,"(%d,%d) ",iv->i_start,iv->i_stop);
			}
			fprintf(stderr,"} \n");
			fprintf(stderr,"busy = {");
			for (iv = al->al_busy; iv != (interv_p) 0;
			     iv = iv->i_next) {
				fprintf(stderr,"(%d,%d) ",iv->i_start,iv->i_stop);
			}
			fprintf(stderr,"} \n");
			fprintf(stderr,"profits = %d\n",al->al_profits);
			fprintf(stderr,"dummy local = %ld\n",al->al_dummy);
			fprintf(stderr,"regnr = %d\n",al->al_regnr);
		}
	}
}


short regs_needed[4];
stat_regusage(list)
	alloc_p list;
{
	int i;
	alloc_p x;

	for (i = 0; i < 4; i++) {
		regs_needed[i] = 0;
	}
	for (x = list; x != (alloc_p) 0; x = x->al_next) {
		regs_needed[x->al_regtype]++;
	}
	/* fprintf(stderr, "data regs:%d\n",regs_needed[reg_any]); */
	/* fprintf(stderr, "address regs:%d\n",regs_needed[reg_pointer]); */
}

		

int cnt_regtypes[reg_float+1];

statistics(items)
	item_p items[];
{
	register item_p item,next;
	int t,r;
	int cnt;

	fprintf(stderr, "\nSTATISTICS\n");
	for (r = 0; r <= reg_float; r++) cnt_regtypes[r] = 0;
	for (t = 0; t < NRITEMTYPES;t++) {
		cnt = 0;
		for (item = items[t]; item != (item_p) 0;item = next) {
			if (useful_item(item)) {
				cnt++;
				cnt_regtypes[item->it_regtype]++;
			}
			next = item->it_next;
		}
		fprintf(stderr, "#%s = %d\n",str_types[t],cnt);
	}
	for (r = 0; r <= reg_float; r++) {
		fprintf(stderr, "#%s = %d\n",str_regtypes[r],cnt_regtypes[r]);
	}
}
