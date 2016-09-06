/*	$Id: local.c,v 1.19 2016/06/27 11:47:06 ragge Exp $	*/
/*
 * Copyright (c) 2003 Anders Magnusson (ragge@ludd.luth.se).
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */


# include "pass1.h"
# include "unicode.h"

#ifdef LANG_CXX
#define P1ND NODE
#define	p1alloc talloc
#define	p1nfree nfree
#define	p1fwalk fwalk
#define	p1tcopy ccopy
#endif

/*	this file contains code which is dependent on the target machine */

P1ND *
clocal(P1ND *p)
{
	struct symtab *q;
	P1ND *r, *l;
	TWORD t;
	int o;

#ifdef PCC_DEBUG
	if (xdebug) {
		printf("clocal\n");
		p1fwalk(p, eprint, 0);
	}
#endif

	switch( o = p->n_op ){
	case NAME:
		/* handle variables */
		if ((q = p->n_sp) == NULL)
			return p; /* Nothing to care about */
		switch (q->sclass) {
		case PARAM:
		case AUTO:
			/* fake up a structure reference */
			r = block(REG, NULL, NULL, PTR+STRTY, 0, 0);
			slval(r, 0);
			r->n_rval = FPREG;
			p = stref(block(STREF, r, p, 0, 0, 0));
			break;
		default:
			break;
		}
		break;

	case PMCONV:
	case PVCONV:
		if( p->n_right->n_op != ICON ) cerror( "bad conversion", 0);
		r = (buildtree(o==PMCONV?MUL:DIV, p->n_left, p->n_right));
		p1nfree(p);
		p = r;
		break;

	case PCONV:
		t = p->n_type;
		if (t == INCREF(CHAR) || t == INCREF(UCHAR) ||
		    t == INCREF(BOOL) || t == INCREF(VOID))
			break;
		l = p->n_left;
		t = l->n_type;
		if (t == INCREF(CHAR) || t == INCREF(UCHAR) ||
		    t == INCREF(BOOL) || t == INCREF(VOID))
			break;
		if (p->n_type <= UCHAR || l->n_type <= UCHAR)
			break; /* must do runtime ptr conv */
		/* if conversion to another pointer type, just remove */
		if (p->n_type > BTMASK && l->n_type > BTMASK)
			goto delp;
		if (l->n_op == ICON && l->n_sp == NULL)
			goto delp;
		break;

	delp:	l->n_type = p->n_type;
		l->n_qual = p->n_qual;
		l->n_df = p->n_df;
		l->n_ap = p->n_ap;
		p = p1nfree(p);
		break;

	case FORCE:
		/* put return value in return reg */
		p->n_op = ASSIGN;
		p->n_right = p->n_left;
		p->n_left = block(REG, NULL, NULL, p->n_type, 0, 0);
		p->n_left->n_rval = RETREG(p->n_type);
		break;

	}

#ifdef PCC_DEBUG
	if (xdebug) {
		printf("clocal end\n");
		p1fwalk(p, eprint, 0);
	}
#endif
	return(p);
}

void
myp2tree(P1ND *p)
{
	struct symtab *sp;
	P1ND *l, *r;
	int o = p->n_op;

	switch (o) {
	case NAME: /* reading from a name must be done with a subroutine */
		if (p->n_type != CHAR && p->n_type != UCHAR)
			break;
		l = buildtree(ADDROF, p1tcopy(p), NULL);
		r = block(NAME, NULL, NULL, INT, 0, 0);

		r->n_sp = lookup(addname("__nova_rbyte"), SNORMAL);
		if (r->n_sp->sclass == SNULL) {
			r->n_sp->sclass = EXTERN;
			r->n_sp->stype = INCREF(p->n_type)+(FTN-PTR);
		}
		r->n_type = r->n_sp->stype;
		r = clocal(r);
		r = optim(buildtree(CALL, r, l));
		*p = *r;
		p1nfree(r);
		break;

	case FCON:
		sp = tmpalloc(sizeof(struct symtab));
		sp->sclass = STATIC;
		sp->sap = 0;
		sp->slevel = 1; /* fake numeric label */
		sp->soffset = getlab();
		sp->sflags = 0;
		sp->stype = p->n_type;
		sp->squal = (CON >> TSHIFT);

		defloc(sp);
		ninval(0, tsize(sp->stype, sp->sdf, sp->sap), p);

		p->n_op = NAME;
		slval(p, 0);
		p->n_sp = sp;
	}
}

/*ARGSUSED*/
int
andable(P1ND *p)
{
	return(1);  /* all names can have & taken on them */
}

/*
 * Return 1 if a variable of type type is OK to put in register.
 */
int
cisreg(TWORD t)
{
	return 1; /* try to put anything in a register */
}

/*
 * return a node, for structure references, which is suitable for
 * being added to a pointer of type t, in order to be off bits offset
 * into a structure
 * t, d, and s are the type, dimension offset, and sizeoffset
 * For nova, return the type-specific index number which calculation
 * is based on its size. For example, char a[3] would return 3.
 * Be careful about only handling first-level pointers, the following
 * indirections must be fullword.
 */
P1ND *
offcon(OFFSZ off, TWORD t, union dimfun *d, struct attr *ap)
{
	register P1ND *p;

	if (xdebug)
		printf("offcon: OFFSZ %ld type %x dim %p siz %ld\n",
		    off, t, d, tsize(t, d, ap));

	p = bcon(off/SZINT);
	if (t == INCREF(CHAR) || t == INCREF(UCHAR) || t == INCREF(VOID))
		slval(p, off/SZCHAR); /* pointer to char */
	return(p);
}

/*
 * Allocate off bits on the stack.  p is a tree that when evaluated
 * is the multiply count for off, t is a NAME node where to write
 * the allocated address.
 */
void
spalloc(P1ND *t, P1ND *p, OFFSZ off)
{
	P1ND *sp;

cerror("spalloc");
	if ((off % SZINT) == 0)
		p =  buildtree(MUL, p, bcon(off/SZINT));
	else if ((off % SZSHORT) == 0) {
		p = buildtree(MUL, p, bcon(off/SZSHORT));
		p = buildtree(PLUS, p, bcon(1));
		p = buildtree(RS, p, bcon(1));
	} else if ((off % SZCHAR) == 0) {
		p = buildtree(MUL, p, bcon(off/SZCHAR));
		p = buildtree(PLUS, p, bcon(3));
		p = buildtree(RS, p, bcon(2));
	} else
		cerror("roundsp");

	/* save the address of sp */
	sp = block(REG, NULL, NULL, PTR+INT, t->n_df, t->n_ap);
	slval(sp, 0);
	sp->n_rval = STKREG;
	t->n_type = sp->n_type;
	ecomp(buildtree(ASSIGN, t, sp)); /* Emit! */

	/* add the size to sp */
	sp = block(REG, NULL, NULL, p->n_type, 0, 0);
	slval(sp, 0);
	sp->n_rval = STKREG;
	ecomp(buildtree(PLUSEQ, sp, p));
}

#ifdef os_none
/*
 * Print out a string of characters.
 * Assume that the assembler understands C-style escape
 * sequences.
 */
void
instring(struct symtab *sp)
{
	unsigned short sh[2];
	char *s;
	TWORD t;
	P1ND *p;

	locctr(STRNG, sp);
	defloc(sp);

	t = BTYPE(sp->stype);
	s = sp->sname;
	if (t == ctype(USHORT)) {
		/* convert to UTF-16 */
		p = xbcon(0, NULL, t);
		while (*s) {
			cp2u16(u82cp(&s), sh);
			if ((glval(p) = sh[0]))
				inval(0, SZSHORT, p);
			if ((glval(p) = sh[1]))
				inval(0, SZSHORT, p);
		}
		slval(p, 0);
		inval(0, SZSHORT, p);
		p1nfree(p);
	} else if (t == ctype(SZINT < 32 ? ULONG : UNSIGNED) ||
	    t == ctype(SZINT < 32 ? LONG : INT)) {
		/* convert to UTF-32 */
		p = xbcon(0, NULL, t);
		while (*s) {
			slval(p, u82cp(&s));
			inval(0, SZINT < 32 ? SZLONG : SZINT, p);
		}
		slval(p, 0);
		inval(0, SZINT < 32 ? SZLONG : SZINT, p);
		p1nfree(p);
	} else if (t == CHAR || t == UCHAR) {
		int chno = 0;
		printf(PRTPREF "\t.TXT \"");
		for (; *s; s++) {
			if (*s == '\\') {
				printf("<%o>", esccon(&s));
			} else
				printf("%c", *s);

			if (chno++ >= 60) {
				printf("\r\n" PRTPREF);
				chno = 0;
			}
		}

		printf("\"\n");
	} else
		cerror("instring %ld", t);
}
#endif

/*
 * print out a constant node
 * mat be associated with a label
 */
int
ninval(CONSZ off, int fsz, P1ND *p)
{
	switch (p->n_type) {
	case FLOAT:
	case DOUBLE:
	case LDOUBLE:
		cerror("ninval");
	}
	return 1;
}

/* make a name look like an external name in the local machine */
char *
exname(char *p)
{
	if (p == NULL)
		return "";
	return p;
}

/*
 * map types which are not defined on the local machine
 */
TWORD
ctype(TWORD type)
{
	switch (BTYPE(type)) {
	case LONGLONG:
		MODTYPE(type,LONG);
		break;

	case ULONGLONG:
		MODTYPE(type,ULONG);
		break;
	case SHORT:
		MODTYPE(type,INT);
		break;
	case USHORT:
		MODTYPE(type,UNSIGNED);
		break;
	}
	return (type);
}

#if 0
/* curid is a variable which is defined but
 * is not initialized (and not a function );
 * This routine returns the storage class for an uninitialized declaration
 */
int
noinit()
{
	return(EXTERN);
}
#endif

void
calldec(P1ND *p, P1ND *q) 
{
}

void
extdec(struct symtab *q)
{
}

#if 0
/* make a common declaration for id, if reasonable */
void
commdec(struct symtab *q)
{
	int off;

	off = tsize(q->stype, q->sdf, q->ssue);
	off = (off+(SZCHAR-1))/SZCHAR;
	printf("	.comm %s,0%o\n", exname(q->soname), off);
}

/* make a local common declaration for id, if reasonable */
void
lcommdec(struct symtab *q)
{
	int off;

	off = tsize(q->stype, q->sdf, q->ssue);
	off = (off+(SZCHAR-1))/SZCHAR;
	if (q->slevel == 0)
		printf("	.lcomm %s,0%o\n", exname(q->soname), off);
	else
		printf("	.lcomm " LABFMT ",0%o\n", q->soffset, off);
}

/*
 * print a (non-prog) label.
 */
void
deflab1(int label)
{
	printf(LABFMT ":\n", label);
}
#endif

/*
 * Give target the opportunity of handling pragmas.
 */
int
mypragma(char *str)
{
	return 0;
}

/*
 * Called when a identifier has been declared, to give target last word.
 * On Nova we put symbols over the size of an int above 24 bytes in
 * offset and leave zeropage for small vars.
 */
void
fixdef(struct symtab *sp)
{
#if 0
	if (sp->sclass != AUTO)
		return; /* not our business */
	if (ISPTR(sp->stype) || sp->stype < LONG)
		return;
	if (sp->soffset >= (MAXZP * SZINT))
		return; /* already above */
	/* have to move */
	/* XXX remember old autooff for reorg of smaller vars */
	if (autooff < MAXZP * SZINT)
		autooff = MAXZP * SZINT;
	oalloc(sp, &autooff);
#endif
}

void
pass1_lastchance(struct interpass *ip)
{
}
