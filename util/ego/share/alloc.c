/*  S H A R E D   F I L E
 *
 *  A L L O C . C
 */



#include <stdio.h>
#include "types.h"
#include "debug.h"
#include "alloc.h"


short * myalloc();
short * malloc();

#ifdef DEBUG

STATIC unsigned maxuse, curruse;

short *newcore(size)
	int size;
{
	if ((curruse += (unsigned)  (size+2)) > maxuse) maxuse = curruse;
	return myalloc(size);
}

oldcore(p,size)
	short *p;
	int size;
{
	curruse -= (size+2);
	free(p);
}

coreusage()
{
	fprintf(stderr,"Maximal core usage (excl. buffers):%u\n",maxuse);
}

#endif


/*
 * The following two sizetables contain the sizes of the various kinds
 * of line and argument structures.
 * The assumption when making the tables was that every non-byte object
 * had to be aligned on an even boundary. On machines where alignment
 * is worse ( for example a long has to be aligned on a longword bound )
 * these tables should be revised.
 * A wasteful but safe approach is to replace every line of them by
 *  sizeof(line_t)
 * and
 *  sizeof(arg_t)
 * respectively.
 */

#ifndef NOTCOMPACT
int lsizetab[] = {
	2*sizeof(line_p)+2*sizeof(byte),
	2*sizeof(line_p)+2*sizeof(byte)+sizeof(short),
	2*sizeof(line_p)+2*sizeof(byte)+sizeof(offset),
	2*sizeof(line_p)+2*sizeof(byte)+sizeof(lab_id),
	2*sizeof(line_p)+2*sizeof(byte)+sizeof(obj_p),
	2*sizeof(line_p)+2*sizeof(byte)+sizeof(proc_p),
	2*sizeof(line_p)+2*sizeof(byte)+sizeof(arg_p),
};

int asizetab[] = {
	sizeof(arg_p)+sizeof(short)+sizeof(offset),
	sizeof(arg_p)+sizeof(short)+sizeof(lab_id),
	sizeof(arg_p)+sizeof(short)+sizeof(obj_p),
	sizeof(arg_p)+sizeof(short)+sizeof(proc_p),
	sizeof(arg_p)+sizeof(short)+sizeof(argb_t),
	sizeof(arg_p)+sizeof(short)+sizeof(short)+sizeof(argb_t),
	sizeof(arg_p)+sizeof(short)+sizeof(short)+sizeof(argb_t),
	sizeof(arg_p)+sizeof(short)+sizeof(short)+sizeof(argb_t)
};
#else
int lsizetab[] = {
	sizeof(struct line),
	sizeof(struct line),
	sizeof(struct line),
	sizeof(struct line),
	sizeof(struct line),
	sizeof(struct line),
	sizeof(struct line)
};

int asizetab[] = {
	sizeof (struct arg),
	sizeof (struct arg),
	sizeof (struct arg),
	sizeof (struct arg),
	sizeof (struct arg),
	sizeof (struct arg),
	sizeof (struct arg),
	sizeof (struct arg)
};
#endif

/*
 * alloc routines:
 * Two parts:
 *   1)	typed alloc and free routines
 *   2) untyped raw core allocation
 */

/*
 * PART 1
 */

line_p	newline(optyp) int optyp; {
	register line_p lnp;
	register kind=optyp;

	lnp = (line_p) newcore(lsizetab[kind]);
	TYPE(lnp) = optyp;
	return(lnp);
}

oldline(lnp) register line_p lnp; {
	register kind=TYPE(lnp)&BMASK;

	if (kind == OPLIST)
		oldargs(ARG(lnp));
	oldcore((short *) lnp,lsizetab[kind]);
}

arg_p newarg(kind) int kind; {
	register arg_p ap;

	ap = (arg_p) newcore(asizetab[kind]);
	ap->a_type = kind;
	return(ap);
}

oldargs(ap) register arg_p ap; {
	register arg_p	next;

	while (ap != (arg_p) 0) {
		next = ap->a_next;
		switch(ap->a_type) {
		case ARGSTRING:
			oldargb(ap->a_a.a_string.ab_next);
			break;
		case ARGICN:
		case ARGUCN:
		case ARGFCN:
			oldargb(ap->a_a.a_con.ac_con.ab_next);
			break;
		}
		oldcore((short *) ap,asizetab[ap->a_type]);
		ap = next;
	}
}

oldargb(abp) register argb_p abp; {
	register argb_p next;

	while (abp != (argb_p) 0) {
		next = abp->ab_next;
		oldcore((short *) abp,sizeof (argb_t));
		abp = next;
	}
}

num_p newnum() {

	return((num_p) newcore(sizeof(struct num)));
}

oldnum(lp) num_p lp; {

	oldcore((short *) lp,sizeof(struct num));
}


sym_p newsym() {

	return((sym_p) newcore(sizeof(struct sym)));
}

oldsym(sp) sym_p sp; {
	oldcore((short *) sp,sizeof(struct sym));
}


prc_p newprc() {
	return((prc_p) newcore(sizeof(struct prc)));
}


oldprc(pp) prc_p pp; {
	oldcore((short *) pp,sizeof(struct prc));
}


argb_p newargb() {

	return((argb_p) newcore(sizeof(argb_t)));
}

obj_p newobject() {
	return((obj_p) newcore(sizeof(struct obj)));
}

oldobjects(op) register obj_p op; {
	register obj_p next;

	while (op != (obj_p) 0) {
		next = op->o_next;
		oldcore((short *) op, sizeof(struct obj));
		op = next;
	}
}

proc_p newproc() {
	return((proc_p) newcore(sizeof(struct proc)));
}

oldproc(p) proc_p p; {
	oldcore((short *) p, sizeof(struct proc));
}

dblock_p newdblock() {
	return((dblock_p) newcore(sizeof(struct dblock)));
}

olddblock(dbl) dblock_p dbl; {
	oldobjects(dbl->d_objlist);
	oldargs(dbl->d_values);
	oldcore((short *) dbl, sizeof(struct dblock));
}


bblock_p newbblock() {
	return((bblock_p) newcore(sizeof(struct bblock)));
}

oldbblock(b) bblock_p b; {
	oldcore((short *) b, sizeof(struct bblock));
}


short **newmap(length) short length; {
	return((short **) newcore((length+1) * sizeof(short *)));
}

oldmap(mp,length) short **mp, length; {
	oldcore((short *) mp, (length+1) * sizeof(short *));
}


elem_p newelem() {
	return((elem_p) newcore(sizeof(struct elemholder)));
}


oldelem(ep) elem_p ep; {
	oldcore((short *) ep, sizeof(struct elemholder));
}


cset newbitvect(n) short n; {
	return((cset) newcore((n-1)*sizeof(int) + sizeof(struct bitvector)));
	/* sizeof(struct bitvector) equals to the size of a struct with
	 * one short, followed by one ALLIGNED int. So the above statement
	 * also works e.g. on a VAX.
	 */
}

oldbitvect(s,n) cset s; short n; {
	oldcore((short *) s, (n-1)*sizeof(int) + sizeof(struct bitvector));
}


loop_p newloop() {
	return((loop_p) newcore(sizeof(struct loop)));
}


oldloop(lp) loop_p lp; {
	oldcore((short *) lp, sizeof(struct loop));
}

use_p newuse() {
	return((use_p) newcore(sizeof(struct use)));
}


olduse(u) use_p u; {
	oldcore((short *) u, sizeof(struct use));
}


change_p newchange() {
	return((change_p) newcore(sizeof(struct change)));
}


oldchange(c) change_p c; {
	oldcore((short *) c, sizeof(struct change));
}


iv_p newiv() {
	return((iv_p) newcore(sizeof(struct iv)));
}

oldiv(i) iv_p i; {
	oldcore((short *) i, sizeof(struct iv));
}


code_p newcinfo() {
	return((code_p) newcore(sizeof(struct code_info)));
}

oldcinfo(c) code_p c; {
	oldcore((short *) c, sizeof(struct code_info));
}


call_p newcall() {
	return((call_p) newcore(sizeof(struct call)));
}

oldcall(c) call_p c; {
	oldcore((short *) c, sizeof(struct call));
}


actual_p newactual() {
	return((actual_p) newcore(sizeof(struct actual)));
}

oldactual(a) actual_p a; {
	oldcore((short *) a, sizeof(struct actual));
}

formal_p newformal() {
	return((formal_p) newcore(sizeof(struct formal)));
}

oldformal(f) formal_p f; {
	oldcore((short *) f, sizeof(struct formal));
}

calcnt_p newcalcnt() {
	return ((calcnt_p) newcore(sizeof(struct calcnt)));
}

oldcalcnt(cc) calcnt_p cc; {
	oldcore((short *) cc, sizeof(struct calcnt));
}

local_p newlocal() {
	return ((local_p) newcore(sizeof(struct local)));
}

oldlocal(lc) local_p lc; {
	oldcore((short *) lc, sizeof(struct local));
}


short *newtable(length) short length; {
	return((short *) newcore((length+1) * sizeof(short)));
}

oldtable(mp,length) short **mp, length; {
	oldcore((short *) mp, (length+1) * sizeof(short));
}

char **newnametab(tablen,namelen)
	short tablen,namelen;
{
	register char **np, **tab;

	tab = (char **) newmap(tablen);
	for (np = &tab[1]; np <= &tab[tablen]; np++) {
		*np = (char *) newcore(namelen);
	}
	return tab;
}

bext_p newcfbx() {
	return ((bext_p) newcore(sizeof(struct bext_cf)));
}

oldcfbx(bxp) bext_p bxp; {
	oldcore((short *) bxp,sizeof(struct bext_cf));
}

lpext_p newcflpx() {
	return ((lpext_p) newcore (sizeof(struct lpext_cf)));
}

oldcflpx(lxp) lpext_p lxp; {
	oldcore((short *) lxp,sizeof(struct lpext_cf));
}

lpext_p newsrlpx() {
	return ((lpext_p) newcore (sizeof(struct lpext_sr)));
}

oldsrlpx(lxp) lpext_p lxp; {
	oldcore((short *) lxp,sizeof(struct lpext_sr));
}

pext_p newilpx() {
	return ((pext_p) newcore(sizeof(struct pext_il)));
}

oldilpx(pxp) pext_p pxp; {
	oldcore((short *) pxp,sizeof(struct pext_il));
}


bext_p newudbx() {
	return ((bext_p) newcore(sizeof(struct bext_ud)));
}

oldudbx(bxp) bext_p bxp; {
	oldcore((short *) bxp,sizeof(struct bext_ud));
}

bext_p newlvbx() {
	return ((bext_p) newcore(sizeof(struct bext_lv)));
}

oldlvbx(bxp) bext_p bxp; {
	oldcore((short *) bxp,sizeof(struct bext_lv));
}

lpext_p newralpx() {
	return ((lpext_p) newcore (sizeof(struct lpext_ra)));
}

oldralpx(lxp) lpext_p lxp; {
	oldcore((short *) lxp,sizeof(struct lpext_ra));
}

bext_p newrabx() {
	return ((bext_p) newcore(sizeof(struct bext_ra)));
}

oldrabx(bxp) bext_p bxp; {
	oldcore((short *) bxp,sizeof(struct bext_ra));
}


cond_p newcondtab(l) int l;
{
	return (cond_p) newcore(l * (sizeof (struct cond_tab)));
}

oldcondtab(tab) cond_p tab;
{
	int i;
	for (i = 0; tab[i].mc_cond != DEFAULT; i++);
	oldcore((short *) tab,((i+1) * sizeof (struct cond_tab)));
}


short *myalloc(size) register size; {
	register short *p,*q;

	p = malloc(size);
	if (p == 0)
		error("out of memory");
	for(q=p;size>0;size -= sizeof(short))
		*q++ = 0;
	return(p);
}
