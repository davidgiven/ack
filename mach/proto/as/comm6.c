/* @(#)comm6.c	1.7 */
/*
 * implement pseudo instructions
 */

#include	"comm0.h"
#include	"comm1.h"
#include	"y.tab.h"

newequate(ip, typ)
register item_t *ip;
register short typ;
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
#endif THREE_PASS
	if (typ == S_UND)
		serror("illegal equate");
	if (pass == PASS_3)
		assert((ip->i_type & S_TYP) == (typ & S_TYP));
	newident(ip, typ);
}

newident(ip, typ)
register item_t *ip;
{
	register flag;
#ifdef GENLAB
	static char genlab[] = GENLAB;
#endif GENLAB

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
#endif THREE_PASS
#ifdef GENLAB
	if (strncmp(ip->i_name, genlab, sizeof(genlab)-1) == 0)
		flag = SYM_LAB;
#endif GENLAB
	if (sflag & flag)
		newsymb(
			ip->i_name,
			ip->i_type & (S_EXT|S_TYP),
			(short)0,
			load(ip)
		);
}

newlabel(ip)
register item_t *ip;
{
#ifdef THREE_PASS
	register addr_t oldval = ip->i_valu;
#endif

	if (DOTSCT == NULL)
		nosect();
	ip->i_type &= ~S_TYP;
	ip->i_type |= DOTTYP;
	if (store(ip, (valu_t) DOTVAL) == 0)
		return;
#ifdef THREE_PASS
	assert(pass != PASS_2 || oldval - ip->i_valu == DOTGAIN);
#endif
}

newsect(ip)
register item_t *ip;
{
	register ushort typ;
	register sect_t *sp = NULL;

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
#ifdef DUK
		ip->i_type = typ;
#else DUK
		ip->i_type = typ | S_EXT;
#endif DUK
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

newbase(base)
valu_t base;
{
#ifdef ASLD
	register sect_t *sp;
	
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
 * NOTE: A rather different solution is used for ASLD and NOLD:
 * ASLD:
 *   -	maximum length of .comm is recorded in i_valu during PASS_1
 *   -	address of .comm is recorded in i_valu in later passes:
 *	assigned at end of PASS_1, corrected for s_gain at end of PASS_2
 *   -	symbol table entries are produced in commfinish()
 * NOLD:
 *   -	i_valu cannot be used since it is needed for relocation info
 *   -	only one .comm with a particular symbol is allowed per module
 *   -	symbol table entries are produced in newcomm()
 */
newcomm(ip, val)
register item_t *ip;
valu_t val;
{
	if (pass == PASS_1) {
		if (DOTSCT == NULL)
			nosect();
		if (val == 0)
			serror("bad size");
		/* printf("declare %s: %o\n", ip->i_name, DOTTYP); */
		if ((ip->i_type & ~S_EXT) == S_UND) {
			--unresolved;
			ip->i_type = S_COM|S_EXT|DOTTYP;
#ifdef ASLD
			ip->i_valu = val;
		} else if (ip->i_type == (S_COM|S_EXT|DOTTYP)) {
			if (ip->i_valu < val)
				ip->i_valu = val;
#endif
		} else
			serror("multiple declared");
	}
#ifndef ASLD
	if (PASS_SYMB == 0)
		return;
	if (pass != PASS_3)
		/*
		 * save symbol table index
		 * for possible relocation
		 */
		ip->i_valu = outhead.oh_nname;
#ifdef DUK
	newsymb(ip->i_name, S_COM|S_EXT|DOTTYP, (short)0, val);
#else DUK
	newsymb(ip->i_name, S_EXT|DOTTYP, (short)0, val);
#endif DUK
#endif
}

switchsect(newtyp)
short newtyp;
{
	register sect_t *sp;
	
	if (sp = DOTSCT)
		sp->s_size = DOTVAL - sp->s_base;
	if (newtyp == S_UND) {
		DOTSCT = NULL;
		DOTTYP = newtyp;
		return;
	}
	assert(newtyp >= S_MIN);
	sp = &sect[newtyp - S_MIN];
	if (pass == PASS_3) {
#ifdef AOUTSEEK
		aoutpart = -1;
		aoutseek[PARTEMIT] = sp->s_foff + sp->s_size - sp->s_zero;
#else
		fseek(aoutfile[PARTEMIT], sp->s_foff + sp->s_size - sp->s_zero, 0);
#endif
	}
	DOTVAL = sp->s_size + sp->s_base;
	DOTSCT = sp;
	DOTTYP = newtyp;
}

align(bytes)
valu_t bytes;
{
	register valu_t gap;
	register sect_t *sp;

	if ((sp = DOTSCT) == NULL)
		nosect();
	if (bytes == 0)
		bytes = ALIGNWORD;
	if (sp->s_lign % bytes)
		if (bytes % sp->s_lign)
			serror("illegal alignment");
		else
			sp->s_lign = bytes;
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
newrelo(s, n)
short s;
{
	struct outrelo outrelo;
#ifdef DUK
	int	iscomm;
#endif DUK

	if (rflag == 0)
		return;
	if (PASS_RELO == 0)
		return;
	s &= ~S_DOT;
	assert((s & ~(S_COM|S_VAR|S_TYP)) == 0);
#ifndef THREE_PASS
	if (s == S_UND)
		serror("bad relocation");
#endif
	/*
	 * always relocation info if S_VAR to solve problems with:
	 *	move	b,d0
	 *	b=a
	 *  a:	.data2	0
	 */
#ifdef DUK
	iscomm = s & S_COM;
#endif DUK
	s &= ~S_COM;
	if ((n & RELPC) == 0 && s == S_ABS)
		return;
	if ((n & RELPC) != 0 && s == DOTTYP)
		return;
	if (pass != PASS_3) {
		outhead.oh_nrelo++;
		return;
	}
	s &= ~S_VAR;
	outrelo.or_type = (char)n;
	outrelo.or_sect = (char)DOTTYP;
#ifndef ASLD
#ifdef DUK
	if (s == S_UND || iscomm) {
#else DUK
	if (s == S_UND) {
#endif DUK
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
	putofmt((char *)&outrelo, SF_RELO, PARTRELO);
}
#endif

newsymb(name, type, desc, valu)
register char *name;
short type;
short desc;
valu_t valu;
{
	struct outname outname;

	if (name && *name == 0)
		name = 0;
	assert(PASS_SYMB);
	if (pass != PASS_3) {
		if (name)
			outhead.oh_nchar += strlen(name)+1;
		outhead.oh_nname++;
		return;
	}
	if (name) {
		AOUTPART(PARTCHAR);
		outname.on_foff = outhead.oh_nchar;
		do {
			AOUTPUTC(*name, PARTCHAR);
			outhead.oh_nchar++;
		} while (*name++);
	} else
		outname.on_foff = 0;
	outname.on_type = type;
	outname.on_desc = desc;
	outname.on_valu = valu & ~((0xFFFFFFFF)<<(8*sizeof(valu_t)));
	putofmt((char *)&outname, SF_NAME, PARTNAME);
}
