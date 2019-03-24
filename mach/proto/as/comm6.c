/* $Id$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* @(#)comm6.c	1.7 */
/*
 * implement pseudo instructions
 */

#include	"comm0.h"
#include	"comm1.h"
#include	"y.tab.h"
#include	"object.h"

static void new_common(item_t *);

void
newequate(item_t *ip, int typ)
{
	typ &= ~S_EXT;
	if (typ & S_COM)
		typ = S_UND;
	else if ((typ & S_VAR) && (typ & S_TYP) != S_ABS)
		typ = S_UND;
#ifdef THREE_PASS
	else if (pass == PASS_1 && typ == S_UND)
		typ = S_VAR;
	else if (pass == PASS_2 && (ip->i_type & S_TYP) == S_UND)
		ip->i_type |= typ;
#endif /* THREE_PASS */
	if (typ == S_UND)
		serror("illegal equate");
	if (pass == PASS_3)
		assert((ip->i_type & S_TYP) == (typ & S_TYP));
	newident(ip, typ);
}

void
newident(item_t *ip, int typ)
{
	int flag;
#ifdef GENLAB
	static char genlab[] = GENLAB;
#endif /* GENLAB */

	if (pass == PASS_1) {
		/* printf("declare %s: %o\n", ip->i_name, typ); */
		if (ip->i_type & ~S_EXT)
			serror("multiple declared");
		else
			--unresolved;
		ip->i_type |= typ;
	}
	if (PASS_SYMB == 0)
		return;
#ifdef THREE_PASS
	if (ip->i_type & S_EXT)
		flag = SYM_EXT;
	else
		flag = SYM_LOC;
#else
	flag = SYM_EXT|SYM_LOC;	/* S_EXT not stable in PASS_1 */
#endif /* THREE_PASS */
#ifdef GENLAB
	if (!(flag & SYM_EXT) &&
	    strncmp(ip->i_name, genlab, sizeof(genlab)-1) == 0)
		flag = SYM_LAB;
#endif /* GENLAB */
	if (sflag & flag)
		newsymb(
			ip->i_name,
			ip->i_type & (S_EXT|S_TYP),
			0,
			load(ip)
		);
}

void
newlabel(item_t *ip)
{
#if defined(THREE_PASS) && !defined(NDEBUG)
	ADDR_T oldval = ip->i_valu;
#endif

	if (DOTSCT == NULL)
		nosect();
	ip->i_type &= ~S_TYP;
	ip->i_type |= DOTTYP;
	if (store(ip, (valu_t) DOTVAL) == 0)
		return;
#ifdef THREE_PASS
	assert(pass != PASS_2 || oldval - (ADDR_T) ip->i_valu == DOTGAIN);
#endif
}

void
newsect(item_t *ip)
{
	int typ;
	sect_t *sp = NULL;

	typ = ip->i_type & S_TYP;
	if (typ == S_UND) {
		/*
		 * new section
		 */
		assert(pass == PASS_1);
		--unresolved;
		typ = outhead.oh_nsect + S_MIN;
		outhead.oh_nsect++;
		if (outhead.oh_nsect > SECTMAX || typ > S_MAX)
			fatal("too many sections");
		sp = &sect[typ - S_MIN];
		sp->s_item = ip;
		sp->s_lign = ALIGNSECT;
#ifndef ASLD
		ip->i_type = typ;
#else
		ip->i_type = typ | S_EXT;
#endif
		ip->i_valu = 0;
	} else if (typ >= S_MIN) {
		sp = &sect[typ - S_MIN];
		if (sp->s_item != ip)
			sp = NULL;
	}
	if (sp == NULL)
		serror("multiple declared");
	else
		switchsect(typ);
}

/*ARGSUSED*/
void
newbase(valu_t base)
{
#ifdef ASLD
	sect_t *sp;
	
	if ((sp = DOTSCT) == NULL)
		nosect();
	if (sp->s_flag & BASED)
		serror("already based");
	sp->s_base = base;
	sp->s_flag |= BASED;
	DOTVAL += base;
#else
	warning(".base ignored");
#endif
}

/*
 * NOTE: A rather different solution is used for ASLD and not ASLD:
 * ASLD, or local commons:
 *   -	maximum length of .comm is recorded in i_valu during PASS_1
 *   -	address of .comm is recorded in i_valu in later passes:
 *	assigned at end of PASS_1, corrected for s_gain at end of PASS_2
 * not ASLD:
 *   -	maximum length of .comm is recorded in i_valu during PASS_1
 *   -	i_valu is used for relocation info during PASS_3
 */
void
newcomm(item_t *ip, valu_t val)
{
	if (pass == PASS_1) {
		if (DOTSCT == NULL)
			nosect();
		if (val == 0)
			serror("bad size");
		/* printf("declare %s: %o\n", ip->i_name, DOTTYP); */
		if ((ip->i_type & ~S_EXT) == S_UND) {
			--unresolved;
			ip->i_type = S_COM|DOTTYP|(ip->i_type&S_EXT);
			ip->i_valu = val;
			new_common(ip);
		} else if (ip->i_type == (S_COM|DOTTYP|(ip->i_type&S_EXT))) {
			if (ip->i_valu < val)
				ip->i_valu = val;
		} else
			serror("multiple declared");
	}
}

void
switchsect(int newtyp)
{
	sect_t *sp;
	
	if ((sp = DOTSCT))
		sp->s_size = DOTVAL - sp->s_base;
	if (newtyp == S_UND) {
		DOTSCT = NULL;
		DOTTYP = newtyp;
		return;
	}
	assert(newtyp >= S_MIN);
	sp = &sect[newtyp - S_MIN];
	DOTVAL = sp->s_size + sp->s_base;
	DOTSCT = sp;
	DOTTYP = newtyp;
}

void
align(valu_t bytes)
{
	valu_t gap;
	sect_t *sp;

	if ((sp = DOTSCT) == NULL)
		nosect();
	if (bytes == 0)
		bytes = ALIGNWORD;
	if (sp->s_lign % bytes)
	{
		if (bytes % sp->s_lign)
		{
			serror("illegal alignment");
		}
		else
		{
			sp->s_lign = bytes;
		}
	}
	if (pass == PASS_1)
		/*
		 * be pessimistic: biggest gap possible
		 */
		gap = bytes - 1;
	else {
		/*
		 * calculate gap correctly;
		 * will be the same in PASS_2 and PASS_3
		 */
		if ((gap = DOTVAL % bytes) != 0)
			gap = bytes - gap;
#ifdef THREE_PASS
		if (pass == PASS_2)
			/*
			 * keep track of gain with respect to PASS_1
			 */
			DOTGAIN += (bytes - 1) - gap;
#endif
	}
	DOTVAL += gap;
	sp->s_zero += gap;
}

#ifdef RELOCATION
void
newrelo(int s, int n)
{
	int	iscomm;
	struct outrelo	outrelo;

	if (rflag == 0)
		return;
	if (PASS_RELO == 0)
		return;
	s &= ~S_DOT;
	assert((s & ~(S_COM|S_VAR|S_TYP)) == 0);
#ifdef ASLD
#ifndef THREE_PASS
	if (s == S_UND)
		serror("bad relocation");
#endif
#endif
	/*
	 * always relocation info if S_VAR to solve problems with:
	 *	move	b,d0
	 *	b=a
	 *  a:	.data2	0
	 * but no relocation info if S_VAR is set, but type is S_ABS.
	 */
	iscomm = s & S_COM;
	s &= ~S_COM;
	if ((n & RELPC) == 0 && ((s & ~S_VAR) == S_ABS))
		return;
	if ((n & RELPC) != 0 && s == DOTTYP
#ifndef ASLD
	    && ! iscomm
#endif
	   )
		return;
	if (pass != PASS_3) {
		outhead.oh_nrelo++;
		return;
	}
	s &= ~S_VAR;
	outrelo.or_type = n;
	outrelo.or_sect = DOTTYP;
#ifndef ASLD
	if (s == S_UND || iscomm) {
		assert(relonami != 0);
		outrelo.or_nami = relonami-1;
		relonami = 0;
	} else
#endif
	if (s < S_MIN) {
		assert(s == S_ABS);
		/*
		 * use first non existing entry (argh)
		 */
		outrelo.or_nami = outhead.oh_nname;
	} else {
		/*
		 * section symbols are at the end
		 */
		outrelo.or_nami = outhead.oh_nname
				- outhead.oh_nsect
				+ (s - S_MIN)
				;
	}
	outrelo.or_addr = (long)DOTVAL;
	wr_relo(&outrelo, 1);
}
#endif

long
new_string(const char *s)
{
	long	r = 0;

	if (s) {
		long len = strlen(s) + 1;

		r = outhead.oh_nchar;
		if (pass == PASS_3) wr_string(s, len);
		outhead.oh_nchar += len;
	}
	return r;
}

void
newsymb(const char *name, int type, int desc, valu_t valu)
{
	struct outname outname;

	if (name && *name == 0)
		name = 0;
	assert(PASS_SYMB);
	if (pass != PASS_3) {
		new_string(name);
		outhead.oh_nname++;
		return;
	}
	nname++;
	outname.on_foff = new_string(name);
	outname.on_type = type;
	outname.on_desc = desc;
	outname.on_valu = valu;
	wr_name(&outname, 1);
}

static void
new_common(item_t *ip)
{
	struct common_t *cp;
	static int nleft = 0;
	static struct common_t *next;

	if (--nleft < 0) {
		next = (struct common_t *) malloc(MEMINCR);
		if (next == 0) {
			fatal("out of memory");
		}
		nleft += (MEMINCR / sizeof (struct common_t));
	}
	cp = next++;
	cp->c_next = commons;
	cp->c_it = ip;
	commons = cp;
}
