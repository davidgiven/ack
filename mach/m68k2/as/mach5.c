/* $Header$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* @(#)mach5.c	1.16 */
/*
 * Motorola 68000/68010 auxiliary functions
 */

ea_1(sz, bits)
{
	register flag;

	if (mrg_1 > 074)
		serror("no specials");
	if ((flag = eamode[mrg_1>>3]) == 0)
		if ((flag = eamode[010 + (mrg_1&07)]) == 0)
			flag = eamode[015 + (sz>>6)];
	if ((mrg_1 & 070) == 010)
		checksize(sz, 2|4);
	bits &= ~flag;
	if (bits)
		serror("bad addressing categorie");
	if (flag & FITW)
		Xfit (fitw(exp_1.val) ||
		      (mrg_1 == 074 && fit16(exp_1.val))
		) ;
	if (flag & FITB) {
		Xfit (fitb(exp_1.val) ||
		      (mrg_1 == 074 && fit8(exp_1.val))
		);
		if (mrg_1 == 074)
			exp_1.val &= 0xFF;
	}
#ifdef RELOCATION
	RELOMOVE(relonami, rel_1);
	if (flag & ~0xFF)
		newrelo(exp_1.typ, (flag>>8) | RELBR | RELWR);
#endif
	if (flag & PUTL)
		emit4(exp_1.val);
	if (flag & PUTW)
		emit2(loww(exp_1.val));
}

ea_2(sz, bits)
{ 
	mrg_1 = mrg_2;
	exp_1 = exp_2;
	RELOMOVE(rel_1, rel_2);
	ea_1(sz, bits);
}

index(hibyte)
{
	Xfit(fitb(exp_2.val));
	exp_2.val = hibyte | lowb(exp_2.val);
}

checksize(sz, bits)
{
	if ((bits & (1 << (sz>>6))) == 0)
		serror("bad size");
}

test68010()
{
	if (model != 1)
		warning("68010 instruction");
}

badoperand()
{
	serror("bad operands");
}

shift_op(opc, sz)
{
	if (mrg_1 < 010 && mrg_2 < 010) {
		emit2((opc&0170470) | sz | mrg_1<<9 | mrg_2);
		return;
	}
	if (exp_1.typ != S_ABS || mrg_1 != 074) {
		badoperand();
		return;
	}
	if (mrg_2 < 010) {
		Xfit(fit3(exp_1.val));
		emit2((opc&0170430) | sz | low3(exp_1.val)<<9 | mrg_2);
		return;
	}
	checksize(sz, 2);
	Xfit(exp_1.val == 1);
	emit2((opc&0177700) | mrg_2);
	ea_2(SIZE_W, MEM|ALT);
}

bitop(opc)
{
	register bits;

	bits = DTA|ALT;
	if (opc == 0 && (mrg_1 < 010 || mrg_2 != 074))
		bits = DTA;
	if (mrg_1 < 010) {
		emit2(opc | 0400 | mrg_1<<9 | mrg_2);
		ea_2(0, bits);
		return;
	}
	if (mrg_1 == 074) {
		emit2(opc | 04000 | mrg_2);
		ea_1(SIZE_W, 0);
		ea_2(0, bits);
		return;
	}
	badoperand();
}

add(opc, sz)
{
	if ((mrg_2 & 070) == 010)
		checksize(sz, 2|4);
	if (
		mrg_1 == 074
		&&
		small(
			exp_1.typ==S_ABS && fit3(exp_1.val),
			sz==SIZE_L ? 4 : 2
		)
	) {
		emit2((opc&0400) | 050000 | low3(exp_1.val)<<9 | sz | mrg_2);
		ea_2(sz, ALT);
		return;
	}
	if (mrg_1 == 074 && (mrg_2 & 070) != 010) {
		emit2((opc&03000) | sz | mrg_2);
		ea_1(sz, 0);
		ea_2(sz, DTA|ALT);
		return;
	}
	if ((mrg_2 & 070) == 010) {
		emit2((opc&0170300) | (mrg_2&7)<<9 | sz<<1 | mrg_1);
		ea_1(sz, 0);
		return;
	}
	if (to_dreg(opc, sz, 0))
		return;
	if (from_dreg(opc, sz, ALT|MEM))
		return;
	badoperand();
}

and(opc, sz)
{
	if (mrg_1 == 074 && mrg_2 >= 076) {	/* ccr or sr */
		if (sz != SIZE_NON)
			checksize(sz, mrg_2==076 ? 1 : 2);
		else
			sz = (mrg_2==076 ? SIZE_B : SIZE_W);
		emit2((opc&07400) | sz | 074);
		ea_1(sz, 0);
		return;
	}
	if (sz == SIZE_NON)
		sz = SIZE_DEF;
	if (mrg_1 == 074) {
		emit2((opc&07400) | sz | mrg_2);
		ea_1(sz, 0);
		ea_2(sz, DTA|ALT);
		return;
	}
	if ((opc & 010000) == 0 && to_dreg(opc, sz, DTA))
		return;
	if (from_dreg(opc, sz, (opc & 010000) ? DTA|ALT : ALT|MEM))
		return;
	badoperand();
}

to_dreg(opc, sz, bits)
{
	if ((mrg_2 & 070) != 000)
		return(0);
	emit2((opc & 0170000) | sz | (mrg_2&7)<<9 | mrg_1);
	ea_1(sz, bits);
	return(1);
}

from_dreg(opc, sz, bits)
{
	if ((mrg_1 & 070) != 000)
		return(0);
	emit2((opc & 0170000) | sz | (mrg_1&7)<<9 | 0400 | mrg_2);
	ea_2(sz, bits);
	return(1);
}

cmp(sz)
{
	register opc;

	if ((mrg_1&070) == 030 && (mrg_2&070) == 030) {
		emit2(0130410 | sz | (mrg_1&7) | (mrg_2&7)<<9);
		return;
	}
	if (mrg_1 == 074 && (mrg_2 & 070) != 010) {
		emit2(06000 | sz | mrg_2);
		ea_1(sz, 0);
		ea_2(sz, DTA|ALT);
		return;
	}
	if (mrg_2 < 020) {
		if (mrg_2 >= 010) {
			checksize(sz, 2|4);
			opc = 0130300 | sz<<1;
			mrg_2 &= 7;
		} else
			opc = 0130000 | sz;
		emit2(opc | mrg_2<<9 | mrg_1);
		ea_1(sz, 0);
		return;
	}
	badoperand();
}

move(sz)
{
	register opc;

	if (mrg_1 > 074 || mrg_2 > 074) {
		move_special(sz);
		return;
	}
	if (sz == SIZE_NON)
		sz = SIZE_DEF;
	if (
		mrg_2<010
		&&
		mrg_1==074
		&&
		sz==SIZE_L
		&&
		small(exp_1.typ==S_ABS && fitb(exp_1.val), 4)
	) {
		emit2(070000 | mrg_2<<9 | lowb(exp_1.val));
		return;
	}
	switch (sz) {
	case SIZE_B:	opc = 010000; break;
	case SIZE_W:	opc = 030000; break;
	case SIZE_L:	opc = 020000; break;
	}
	emit2(opc | mrg_1 | (mrg_2&7)<<9 | (mrg_2&070)<<3);
	ea_1(sz, 0);
	ea_2(sz, ALT);
}

move_special(sz)
{
	if (mrg_2 >= 076) {
		if (sz != SIZE_NON)
			checksize(sz, 2);
		emit2(042300 | (mrg_2==076?0:01000) | mrg_1);
		ea_1(SIZE_W, DTA);
		return;
	}
	if (mrg_1 >= 076) {
		if (mrg_1 == 076)
			test68010();
		if (sz != SIZE_NON)
			checksize(sz, 2);
		emit2(040300 | (mrg_1==076?01000:0) | mrg_2);
		ea_2(SIZE_W, DTA|ALT);
		return;
	}
	if (sz != SIZE_NON)
		checksize(sz, 4);
	if (mrg_1==075 && (mrg_2&070)==010) {
		emit2(047150 | (mrg_2&7));
		return;
	}
	if (mrg_2==075 && (mrg_1&070)==010) {
		emit2(047140 | (mrg_1&7));
		return;
	}
	badoperand();
}

int
reverse(regs, max)
	register int regs;
{
	register int r, i;

	r = regs; regs = 0;
	for (i = max; i > 0; i--) {
		regs <<= 1;
		if (r & 1) regs++;
		r >>= 1;
	}
	return regs;
}

movem(dr, sz, regs)
{
	register i;

	if ((mrg_2>>3) == 04) {
		regs = reverse(regs, 16);
	}
	checksize(sz, 2|4);
	if ((mrg_2>>3)-3 == dr)
		badoperand();
	emit2(044200 | dr<<10 | (sz & 0200) >> 1 | mrg_2);
	emit2(regs);
	i = CTR;
	if (dr == 0 && (mrg_2&070) == 040)
		i = MEM;
	if (dr != 0 && (mrg_2&070) == 030)
		i = MEM;
	if (dr == 0)
		i |= ALT;
	ea_2(sz, i);
}

movep(sz)
{
	checksize(sz, 2|4);
	if (mrg_1<010 && (mrg_2&070)==050) {
		emit2(0610 | (sz & 0200)>>1 | mrg_1<<9 | (mrg_2&7));
		ea_2(sz, 0);
		return;
	}
	if (mrg_2<010 && (mrg_1&070)==050) {
		emit2(0410 | (sz & 0200)>>1 | mrg_2<<9 | (mrg_1&7));
		ea_1(sz, 0);
		return;
	}
	badoperand();
}

branch(opc, exp)
expr_t exp;
{
	register sm;

	exp.val -= (DOTVAL + 2);
	if ((pass == PASS_2) 
	    &&
	    (exp.val > 0)
	    &&
	    ((exp.typ & S_DOT) == 0)
	   )
		exp.val -= DOTGAIN;
	sm = fitb(exp.val);
	if ((exp.typ & ~S_DOT) != DOTTYP)
		sm = 0;
	if (small(sm,2)) {
		if (exp.val == 0)
			emit2(047161);	/* NOP */
		else
			emit2(opc | lowb(exp.val));
	} else {
		Xfit(fitw(exp.val));
		emit2(opc);
#ifdef RELOCATION
		newrelo(exp.typ, RELPC|RELO2|RELBR|RELWR);
#endif
		emit2(loww(exp.val));
	}
}

ea5x73(rg, sz)
{
	if ((exp_2.typ & ~S_DOT) == DOTTYP) {
		/* pc relative with index */
		if (sz == SIZE_NON)
			sz = SIZE_DEF;
		exp_2.val -= (DOTVAL + 2);
		mrg_2 = 073;
		checksize(sz, 2|4);
		index(rg<<12 | (sz&0200)<<4);
		return;
	}
	/* displacement */
	mrg_2 = 050 | rg;
	if (pass == PASS_1)	/* tricky, maybe 073 in next passes */
		return;
	if ((rg & 010) == 0 || sz != SIZE_NON)
		badoperand();
}

ea707172(sz)
{
	register sm;

	mrg_2 = 071;
	switch (sz) {
	case SIZE_B:
		badoperand();
	case SIZE_W:
		mrg_2 = 070;
	case SIZE_L:
		return;
	case SIZE_NON:
		break;
	}
	if (pass == PASS_1) {
		/*
		 * reserve a bit in the bittable
		 * in the following passes one call to small()
		 * will be done, but we don't know which one
		 * since exp_2.typ cannot be trusted
		 */
		small(0, 2);
		return;
	}
	if ((exp_2.typ & ~S_DOT) == DOTTYP) {
		sm = fitw(exp_2.val-(DOTVAL+2));
		sm = small(sm, 2);
		if (sm) {	/* pc relative */
			exp_2.val -= (DOTVAL+2);
			mrg_2 = 072;
		}
	} else {
		sm = fitw(exp_2.val);
#ifdef ASLD
		if (exp_2.typ & S_VAR)
			sm = 0;
#else
		if (exp_2.typ != S_ABS)
			sm = 0;
#endif ASLD
		sm = small(sm, 2);
		if (sm)
			mrg_2 = 070;
	}
}

ea6x(rg, ir, sz)
{
	mrg_2 = 060 | rg;
	checksize(sz, 2|4);
	index(ir<<12 | (sz&0200)<<4);
}

ea72()
{
	mrg_2 = 072;
	exp_2.val -= (DOTVAL + 2);
}

ea73(ri, sz)
{
	mrg_2 = 073;
	exp_2.val -= (DOTVAL + 2);
	checksize(sz, 2|4);
	index(ri<<12 | (sz&0200)<<4);
}

Xnofit()
{
	if (pass == PASS_3) serror("too big");
}

fbranch(opc, exp)
expr_t exp;
{
	register sm;

	exp.val -= (DOTVAL + 2);
	if ((pass == PASS_2) 
	    &&
	    (exp.val > 0)
	    &&
	    ((exp.typ & S_DOT) == 0)
	   )
		exp.val -= DOTGAIN;
	sm = fitw(exp.val);
	if ((exp.typ & ~S_DOT) != DOTTYP)
		sm = 0;
	if (small(sm,2)) {
		emit2(0170200|co_id|opc);
		emit2(loww(exp.val));
		return;
	}
	emit2(0170300|co_id|opc); /* 4 byte offset */
#ifdef RELOCATION
	newrelo(exp.typ, RELPC|RELO4|RELBR|RELWR);
#endif
	emit4(exp.val);
}

ch_sz_dreg(size, mode)
{
	if (mode == 0 &&
	    (size == FSIZE_X || size == FSIZE_P || size == FSIZE_D))
		serror("illegal size for data register");
}

check_fsize(sz, size)
{
	if (sz != size) serror("bad size");
}
