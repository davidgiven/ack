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
#include "ego/share/types.h"
#include "ego/share/mach.h"
#include "ego/share/debug.h"
#include "ego/share/global.h"
#include "ego/share/files.h"
#include "ego/share/get.h"
#include "ego/share/put.h"
#include "ego/share/cset.h"
#include "ego/share/lset.h"
#include "ego/share/map.h"
#include "ego/share/alloc.h"
#include "ego/share/go.h"
#include "ra.h"
#include "ra_items.h"
#include "ra_allocl.h"
#include "ra_lifet.h"
#include "ra_profits.h"
#include "ra_pack.h"
#include "ra_xform.h"

#define newrabx()   (bext_p) newstruct(bext_ra)
#define newralpx()  (lpext_p) newstruct(lpext_ra)
#define oldrabx(x)  oldstruct(bext_ra, x)
#define oldralpx(x) oldstruct(lpext_ra, x)

static void stat_regusage(alloc_p list);

short alloc_id;
static item_p items[NRITEMTYPES];
int nrinstrs;
line_p* instrmap;

cond_p alocaltab[NRREGTYPES][NRREGTYPES], alocaddrtab[NRREGTYPES][NRREGTYPES], aconsttab,
    adconsttab, aglobaltab, aproctab;
cond_p olocaltab[NRREGTYPES], olocaddrtab[NRREGTYPES], oconsttab, odconsttab, oglobaltab, oproctab;
cond_p regsav_cost;

short regs_available[] = {
	/* Actually machine dependent; this is for vax2 */
	3, /* reg_any i.e. data regs */
	0, /* reg_loop */
	3, /* reg_pointer i.e. address reg. */
	0 /* reg_float */
};

short use_any_as_pointer = 0;

static int register_class(config_setting_t* setting, const char* field)
{
	const char* name = mach_string(setting, field);
	if (!strcmp(name, "general"))
		return reg_any;
	if (!strcmp(name, "pointer"))
		return reg_pointer;
	if (!strcmp(name, "float"))
		return reg_float;
	error("unknown register class %s", name);
}

static void get_atab(config_setting_t* settings, cond_p tab[NRREGTYPES][NRREGTYPES])
{
	int i;
	for (i = 0; i < config_setting_length(settings); i++)
	{
		config_setting_t* entry = config_setting_get_elem(settings, i);
		int from = register_class(entry, "from");
		int to = register_class(entry, "to");
		tab[from][to] = mach_condtab(config_setting_get_member(entry, "rules"));
	}
}

static void get_otab(config_setting_t* settings, cond_p tab[NRREGTYPES])
{
	int i;
	for (i = 0; i < config_setting_length(settings); i++)
	{
		config_setting_t* entry = config_setting_get_elem(settings, i);
		tab[register_class(entry, "register_class")]
		    = mach_condtab(config_setting_get_member(entry, "rules"));
	}
}

static void ra_machinit(config_t* config)
{
	config_setting_t* regs = mach_setting(config, "register_allocation.registers");
	config_setting_t* saves;
	int i, length;

	regs_available[reg_any] = mach_int(regs, "general");
	regs_available[reg_pointer] = mach_int(regs, "pointer");
	regs_available[reg_float] = mach_int(regs, "float");
	use_any_as_pointer = mach_bool(regs, "general_as_pointer");
	get_atab(mach_setting(config, "register_allocation.scores.local_variable"), alocaltab);
	get_atab(mach_setting(config, "register_allocation.scores.local_address"), alocaddrtab);
	aconsttab = mach_condtab(mach_setting(config, "register_allocation.scores.constant"));
	adconsttab = mach_condtab(mach_setting(config, "register_allocation.scores.double_constant"));
	aglobaltab = mach_condtab(mach_setting(config, "register_allocation.scores.global_address"));
	aproctab = mach_condtab(mach_setting(config, "register_allocation.scores.procedure_address"));
	get_otab(mach_setting(config, "register_allocation.opening_costs.local_variable"), olocaltab);
	get_otab(mach_setting(config, "register_allocation.opening_costs.local_address"), olocaddrtab);
	oconsttab = mach_condtab(mach_setting(config, "register_allocation.opening_costs.constant"));
	odconsttab
	    = mach_condtab(mach_setting(config, "register_allocation.opening_costs.double_constant"));
	oglobaltab
	    = mach_condtab(mach_setting(config, "register_allocation.opening_costs.global_address"));
	oproctab
	    = mach_condtab(mach_setting(config, "register_allocation.opening_costs.procedure_address"));

	saves = mach_setting(config, "register_allocation.save_costs");
	length = config_setting_length(saves);
	regsav_cost = newcondtab(length);
	for (i = 0; i < length; i++)
	{
		config_setting_t* cost = config_setting_get_elem(saves, i);
		/* Save costs are indexed directly by number of registers. */
		regsav_cost[i].mc_cond = (i == length - 1) ? DEFAULT : i + 1;
		regsav_cost[i].mc_tval = mach_int(cost, "time");
		regsav_cost[i].mc_sval = mach_int(cost, "space");
	}
}

static bblock_p header(loop_p lp)
{
	/* Try to determine the 'header' block of loop lp.
	 * If 'e' is the entry block of loop L, then block 'b' is
	 * called the header block of L, iff:
	 *	SUCC(b) = {e} & PRED(e) = {b}
	 * If lp has no header block, 0 is returned.
	 */

	bblock_p x = lp->lp_entry->b_idom;

	if (x != (bblock_p)0 && Lnrelems(x->b_succ) == 1
	    && (bblock_p)Lelem(Lfirst(x->b_succ)) == lp->lp_entry)
	{
		return x;
	}
	return (bblock_p)0;
}

static void ra_extproc(proc_p p)
{
	/* Allocate the extended data structures for procedure p */

	loop_p lp;
	Lindex pi;
	bblock_p b;

	for (pi = Lfirst(p->p_loops); pi != (Lindex)0; pi = Lnext(pi, p->p_loops))
	{
		lp = (loop_p)Lelem(pi);
		lp->lp_extend = newralpx();
		lp->LP_HEADER = header(lp);
	}
	for (b = p->p_start; b != (bblock_p)0; b = b->b_next)
	{
		b->b_extend = newrabx();
	}
}

static void ra_cleanproc(proc_p p)
{
	/* Allocate the extended data structures for procedure p */

	loop_p lp;
	Lindex pi;
	bblock_p b;

	for (pi = Lfirst(p->p_loops); pi != (Lindex)0; pi = Lnext(pi, p->p_loops))
	{
		lp = (loop_p)Lelem(pi);
		oldralpx(lp->lp_extend);
	}
	for (b = p->p_start; b != (bblock_p)0; b = b->b_next)
	{
		oldrabx(b->b_extend);
	}
}

static void loop_blocks(proc_p p)
{
	/* Compute the LP_BLOCKS sets for all loops of p */

	bblock_p b;
	Lindex i;

	for (b = p->p_start; b != (bblock_p)0; b = b->b_next)
	{
		for (i = Lfirst(b->b_loops); i != (Lindex)0; i = Lnext(i, b->b_loops))
		{
			Ladd(b, &(((loop_p)Lelem(i))->LP_BLOCKS));
		}
	}
}

static void make_instrmap(proc_p p, line_p map[])
{
	/* make the instructions map of procedure p */

	bblock_p b;
	line_p l;
	int i = 0;

	for (b = p->p_start; b != (bblock_p)0; b = b->b_next)
	{
		b->B_BEGIN = i; /* number of first instruction */
		for (l = b->b_start; l != (line_p)0; l = l->l_next)
		{
			map[i++] = l;
		}
		b->B_END = i - 1; /* number of last instruction */
	}
}

static bool useful_item(item_p item)
{
	/* See if it may be useful to put the item in a register.
	 * A local variable may always be put in a register.
	 * Other items must be used at least twice.
	 */

	int nruses = Lnrelems(item->it_usage);
	assert(nruses > 0); /* otherwise it would not be an item! */
	return nruses > 1 || item->it_type == LOCALVAR;
}

static void cleantimeset(lset s)
{
	Lindex i;
	time_p t;

	for (i = Lfirst(s); i != (Lindex)0; i = Lnext(i, s))
	{
		t = (time_p)Lelem(i);
		oldtime(t);
	}
	Ldeleteset(s);
}

static item_p cat_items(item_p items[])
{
	/* Make one item list out of an array of itemlists.
	 * Remove items that are used only once.
	 */

	item_p it;
	item_p *ip, head, next;
	int t;

	ip = &head;
	for (t = 0; t < NRITEMTYPES; t++)
	{
		for (it = items[t]; it != (item_p)0; it = next)
		{
			next = it->it_next;
			if (!it->it_desirable || !useful_item(it))
			{
				cleantimeset(it->it_usage);
				olditem(it);
			}
			else
			{
				*ip = it;
				ip = &it->it_next;
			}
		}
	}
	*ip = (item_p)0;
	return head;
}

static void clean_interval(interv_p list)
{
	interv_p x, next;

	for (x = list; x != (interv_p)0; x = next)
	{
		next = x->i_next;
		oldinterval(x);
	}
}

static void clean_allocs(alloc_p list)
{
	alloc_p x, next;

	for (x = list; x != (alloc_p)0; x = next)
	{
		next = x->al_next;
		clean_interval(x->al_timespan);
		Cdeleteset(x->al_rivals);
		Ldeleteset(x->al_inits);
		clean_interval(x->al_busy);
		clean_allocs(x->al_mates);
		oldalloc(x);
	}
}

static void cleanitems(item_p list)
{
	item_p x, next;

	for (x = list; x != (item_p)0; x = next)
	{
		next = x->it_next;
		cleantimeset(x->it_usage);
		olditem(x);
	}
}

/* ARGSUSED */
void ra_initialize(void* null)
{
	init_replacements(ps, ws);
}

void ra_optimize(void* vp)
{
	proc_p p = vp;
	item_p itemlist;
	alloc_p alloclist, packed, unpacked;
	offset locls;
	bool time_opt = (time_space_ratio == 100);

	if (IS_ENTERED_WITH_GTO(p))
		return;
	ra_extproc(p);
	loop_blocks(p);
	alloc_id = 0;
	locls = p->p_localbytes;
	build_itemlist(p, items, &nrinstrs);
	instrmap = (line_p*)newmap(nrinstrs - 1); /* map starts counting at 0 */
	make_instrmap(p, instrmap);
	build_lifetimes(items);
	/* print_items(items,p); */
	/* statistics(items); */
	itemlist = cat_items(items); /* make one list */
	alloclist = build_alloc_list(p, Lnrelems(p->p_loops), itemlist);
	build_rivals_graph(alloclist);
	compute_profits(alloclist, time_opt);
	/* print_allocs(alloclist); */
	pack(alloclist, time_opt, &packed, &unpacked, p);
	stat_regusage(packed);
	xform_proc(p, packed, nrinstrs, instrmap);
	/* print_allocs(packed); */
	p->p_localbytes = locls;
	/* don't really allocate dummy local variables! */
	rem_locals(p, packed);
	rem_formals(p, packed);
	/* remove storage for real locals that
	 *are always put in register .
	 */
	clean_allocs(unpacked);
	clean_allocs(packed);
	cleanitems(itemlist);
	oldmap((void**)instrmap, nrinstrs - 1);
	ra_cleanproc(p);
}

int main(int argc, char* argv[])
{
	go(argc, argv, ra_initialize, ra_optimize, ra_machinit, no_action);
	exit(0);
}

/***************************************************************************/
/***************************************************************************/
/***************************************************************************/

/* debugging stuff */

char* str_types[] = { "local variable",     "addr. of local", "addr. of external",
	                  "addr. of procedure", "constant",       "double constant" };

char* str_regtypes[] = { "any", "loop", "pointer", "float" };

/*
 * All calls to print_items() and print_allocs() are in comments!
 */
#if 0
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
#endif

static short regs_needed[4];
static void stat_regusage(alloc_p list)
{
	int i;
	alloc_p x;

	for (i = 0; i < 4; i++)
	{
		regs_needed[i] = 0;
	}
	for (x = list; x != (alloc_p)0; x = x->al_next)
	{
		regs_needed[x->al_regtype]++;
	}
	/* fprintf(stderr, "data regs:%d\n",regs_needed[reg_any]); */
	/* fprintf(stderr, "address regs:%d\n",regs_needed[reg_pointer]); */
}

/*
 * All calls to statistics() are in comments!
 */
#if 0
int cnt_regtypes[reg_float+1];

statistics(items)
	item_p items[];
{
	item_p item,next;
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
#endif
