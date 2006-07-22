/* $Id$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/*  S H A R E D   F I L E
 *
 *  A L L O C . C
 */



#include <stdlib.h>
#include <stdio.h>
#include "types.h"
#include "debug.h"
#include "alloc.h"


char * myalloc();

#ifdef DEBUG

STATIC unsigned maxuse, curruse;

char *newcore(size)
	int size;
{
	if ((curruse += (unsigned)  (size+2)) > maxuse) maxuse = curruse;
	return myalloc(size);
}

oldcore(p,size)
	char *p;
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
	oldcore((char *) lnp,lsizetab[kind]);
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
		oldcore((char *) ap,asizetab[ap->a_type]);
		ap = next;
	}
}

oldargb(abp) register argb_p abp; {
	register argb_p next;

	while (abp != (argb_p) 0) {
		next = abp->ab_next;
		oldcore((char *) abp,sizeof (argb_t));
		abp = next;
	}
}

oldobjects(op) register obj_p op; {
	register obj_p next;

	while (op != (obj_p) 0) {
		next = op->o_next;
		oldcore((char *) op, sizeof(struct obj));
		op = next;
	}
}

olddblock(dbl) dblock_p dbl; {
	oldobjects(dbl->d_objlist);
	oldargs(dbl->d_values);
	oldcore((char *) dbl, sizeof(struct dblock));
}


short **newmap(length) short length; {
	return((short **) newcore((length+1) * sizeof(short *)));
}

/*ARGSUSED1*/
oldmap(mp,length) short **mp, length; {
	oldcore((char *) mp, (length+1) * sizeof(short *));
}


cset newbitvect(n) short n; {
	return((cset) newcore((n-1)*sizeof(int) + sizeof(struct bitvector)));
	/* sizeof(struct bitvector) equals to the size of a struct with
	 * one short, followed by one ALLIGNED int. So the above statement
	 * also works e.g. on a VAX.
	 */
}

/*ARGSUSED1*/
oldbitvect(s,n) cset s; short n; {
	oldcore((char *) s, (n-1)*sizeof(int) + sizeof(struct bitvector));
}


short *newtable(length) short length; {
	return((short *) newcore((length+1) * sizeof(short)));
}

/*ARGSUSED1*/
oldtable(mp,length) short **mp, length; {
	oldcore((char *) mp, (length+1) * sizeof(short));
}

cond_p newcondtab(l) int l;
{
	return (cond_p) newcore(l * (sizeof (struct cond_tab)));
}

oldcondtab(tab) cond_p tab;
{
	int i;
	for (i = 0; tab[i].mc_cond != DEFAULT; i++);
	oldcore((char *) tab,((i+1) * sizeof (struct cond_tab)));
}


char *myalloc(size) register size; {
	register char *p;

	p = calloc((unsigned) size, 1);
	if (p == 0)
		error("out of memory");
	return(p);
}
