/* $Header$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/*
 * Motorola 68020 auxiliary functions
 */

/* With pc-relative modes the offset is calulated from the address of the 
 * extension word. This address is not known until the instruction opcode(s)
 * have been emitted. Since this address is unknown, the offset from pc
 * cannot be calculated correctly, so it cannot immediately be decided whether
 * to use mode 072 (pc-relative with 16 bit offset) or mode 073 (pc_relative
 * with possibly 32 bit offset) Because of this, instruction opcodes
 * are not really emitted right away, but temporarily stored. This way
 * the address of the extension word is known so the offset can be calculated
 * correctly and it then can be decided to use mode 072 or 073; this can be
 * altered in the instruction opcode, if necessary. For the sake of consistency
 * the effective address(es) are also stored temporarily. The instruction is
 * then emitted in one go, by emit_instr().
 */

emit_instr()
{
	register instr_t *ip;

	for (ip=instr; ip<instrp; emit2((ip++)->i_word)) {
#ifdef RELOCATION
		RELOMOVE(relonami, ip->i_relonami);
		if (ip->i_reloinfo)
			newrelo(ip->i_relotype, ip->i_reloinfo | RELBR | RELWR);
#endif
	}
}


#ifdef RELOCATION
t_emit2(word, relotype, reloinfo, relnm)
short word;
short relotype;
valu_t relnm;
#else
t_emit2(word)
short word;
#endif
{
#ifdef RELOCATION
	if (instrp->i_reloinfo = reloinfo) {
		RELOMOVE(instrp->i_relonami, relnm);
		instrp->i_relotype = relotype;
	}
#endif
	instrp->i_word = word;
	instrp++;
	dot_offset += 2;
}

#ifdef RELOCATION
t_emit4(words, relotype, reloinfo, relnm)
long words;
short relotype;
valu_t relnm;
#else
t_emit4(words)
long words;
#endif
{
	T_EMIT2((short)(words>>16), relotype, reloinfo, relnm);
	T_EMIT2((short)(words), 0, 0, 0);
}

ea_1(sz, bits)
{
		/* Because displacements come in three sizes (null displacement,
		 * word and long displacement), each displacement requires
		 * two bits in the bittable, so two calls to small. Sometimes
		 * one of these calls is a dummy call.
		 */

	register flag;
	register sm, sm1, sm2;

	if (mrg_1 > 074)
		serror("no specials");
	if ((flag = eamode[mrg_1>>3]) == 0)
		if ((flag = eamode[010 + (mrg_1&07)]) == 0)
			flag = eamode[015 + (sz>>6)];
	if ((mrg_1 & 070) == 010)
		checksize(sz, 2|4);
	bits &= ~flag;
	if (bits)
		serror("bad addressing category");
	if (mrg_1==073 && (ffew_1 & 0200) == 0 && (bd_1.typ & ~S_DOT) == DOTTYP)
		bd_1.val -= (DOTVAL + dot_offset);

	if ( (mrg_1==073) || (mrg_1&070)==060 ) {
		sm = (
			(mrg_1==073 && (bd_1.typ & ~S_DOT)==DOTTYP)
		      ||
			(bd_1.typ == S_ABS)
		     );
		if (small(sm && fitw(bd_1.val), 2)) {
			sm = (
				(sm1 = ((ffew_1 & 0307)==0 && fitb(bd_1.val)))
			      ||
				(sm2 = ((ffew_1 & 0307)==0100 && mrg_1==073))
			      ||
				(bd_1.val==0)
			     );
			if (small(sm,2)) {
				if (sm1) {	/* brief format extension */
				    T_EMIT2((ffew_1&0177000) | lowb(bd_1.val),
								    0, 0, 0);
				    return;
				}
				if (sm2) {
					/* change mode to 072 in opcode word */
				    instr->i_word &= ~1;
				    T_EMIT2(loww(bd_1.val), 0, 0, 0);
				    return;
				}
				ffew_1 &= ~040;	/* null displacement */
			}
			else
				ffew_1 &= ~020;		/* word displacement */
		} else
			sm = small(0,2); /* dummy call */

		if (ffew_1 & 3) {
			sm = (od_1.typ == S_ABS);
			if (small(sm && fitw(od_1.val), 2))
				ffew_1 &= small(od_1.val==0, 2) ? ~2 : ~1;
			else
				sm = small(0,2); /* dummy call */
		}

		assert((ffew_1 & 0410) == 0400);
		T_EMIT2(ffew_1, 0, 0, 0);

		assert(ffew_1 & 060);
		switch(ffew_1 & 060) {
		case 020:
			break;
		case 040:
			T_EMIT2(loww(bd_1.val), 0, 0, 0);
			break;
		case 060:
			T_EMIT4(    bd_1.val,
				    bd_1.typ,
				    (mrg_1 == 073 && (ffew_1 & 0200) == 0)
					? RELPC|RELO4
					: RELO4,
				    bd_rel1
				);
		}

		if (ffew_1 & 3) {
			switch(ffew_1 & 3) {
			case 1:
				break;
			case 2:
				T_EMIT2(loww(od_1.val), 0, 0, 0);
				break;
			case 3:
				T_EMIT4(od_1.val, od_1.typ, RELO4, od_rel1);
			}
		}
		return; /* mode 060 and 073 have been dealt with */
	}

	if (flag & FITW)
		if (
			! fitw(bd_1.val)
			&&
			(mrg_1 != 074 || ! fit16(bd_1.val))
		)
			nofit();
	if (flag & FITB) {
		if (
			! fitb(bd_1.val)
			&&
			(mrg_1 != 074 || ! fit8(bd_1.val))
		)
			nofit();
		if (mrg_1 == 074)
			bd_1.val &= 0xFF;
	}
	if (flag & PUTL)
		T_EMIT4(bd_1.val, bd_1.typ, (flag>>8), bd_rel1);
	if (flag & PUTW)
		T_EMIT2(loww(bd_1.val), bd_1.typ, (flag>>8), bd_rel1);
}

ea_2(sz, bits)
{ 
	mrg_1  = mrg_2;
	bd_1   = bd_2;
	od_1   = od_2;
	ffew_1 = ffew_2;
	RELOMOVE(bd_rel1, bd_rel2);
	RELOMOVE(od_rel1, od_rel2);
	ea_1(sz, bits);
}

checksize(sz, bits)
{
	if ((bits & (1 << (sz>>6))) == 0)
		serror("bad size");
}

check_fsize(sz, size)
{
	if (sz != size)
		serror("bad size");
}

ch_sz_dreg(size, mode)
{
	if (mode == 0 &&
	    (size == FSIZE_X || size == FSIZE_P || size == FSIZE_D))
		serror("illegal size for data register");
}

checkscale(val)
valu_t val;
{
	int v = val;

	if (v != val) v = 0;
	switch(v) {
	case 1:		return 0;
	case 2:		return 1<<9;
	case 4:		return 2<<9;
	case 8:		return 3<<9;
	default:	serror("bad scale"); return 0;
	}
}

badoperand()
{
	serror("bad operand(s)");
}

shift_op(opc, sz)
{
	if (mrg_1 < 010 && mrg_2 < 010) {
		T_EMIT2((opc & 0170470) | sz | mrg_1<<9 | mrg_2, 0, 0, 0);
		return;
	}
	if (bd_1.typ != S_ABS || mrg_1 != 074) {
		badoperand();
		return;
	}
	if (mrg_2 < 010) {
		fit(fit3(bd_1.val));
		T_EMIT2((opc & 0170430) | sz | low3(bd_1.val)<<9 | mrg_2,0,0,0);
		return;
	}
	checksize(sz, 2);
	fit(bd_1.val == 1);
	T_EMIT2((opc & 0177700) | mrg_2, 0, 0, 0);
	ea_2(SIZE_W, MEM|ALT);
}

bitop(opc)
{
	register bits;

	bits = DTA|ALT;
	if (opc == 0 && (mrg_1 < 010 || mrg_2 != 074))
		bits = DTA;
	if (mrg_1 < 010) {
		T_EMIT2(opc | 0400 | mrg_1<<9 | mrg_2, 0, 0, 0);
		ea_2(0, bits);
		return;
	}
	if (mrg_1 == 074) {
		T_EMIT2(opc | 04000 | mrg_2, 0, 0, 0);
		ea_1(SIZE_W, 0);
		ea_2(0, bits);
		return;
	}
	badoperand();
}

bitfield(opc, extension)
{
	T_EMIT2(opc | mrg_2, 0, 0, 0);
	T_EMIT2(extension, 0, 0, 0);
	ea_2(SIZE_L, (mrg_2 < 010) ? 0 : (CTR | ALT));
}

add(opc, sz)
{
	if ((mrg_2 & 070) == 010)
		checksize(sz, 2|4);
	if (
		mrg_1 == 074
		&&
		small(
			bd_1.typ==S_ABS && fit3(bd_1.val),
			sz==SIZE_L ? 4 : 2
		     )
	   ) {
		T_EMIT2((opc&0400) | 050000 | low3(bd_1.val)<<9 | sz | mrg_2,
								0, 0, 0);
		ea_2(sz, ALT);
		return;
	}
	if (mrg_1 == 074 && (mrg_2 & 070) != 010) {
		T_EMIT2((opc&03000) | sz | mrg_2, 0, 0, 0);
		ea_1(sz, 0);
		ea_2(sz, DTA|ALT);
		return;
	}
	if ((mrg_2 & 070) == 010) {
		T_EMIT2((opc&0170300) | (mrg_2&7)<<9 | sz<<1 | mrg_1, 0, 0, 0);
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
		T_EMIT2((opc&07400) | sz | 074, 0, 0, 0);
		ea_1(sz, 0);
		return;
	}
	if (sz == SIZE_NON)
		sz = SIZE_DEF;
	if (mrg_1 == 074) {
		T_EMIT2((opc&07400) | sz | mrg_2, 0, 0, 0);
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
	T_EMIT2((opc & 0170000) | sz | (mrg_2&7)<<9 | mrg_1, 0, 0, 0);
	ea_1(sz, bits);
	return(1);
}

from_dreg(opc, sz, bits)
{
	if ((mrg_1 & 070) != 000)
		return(0);
	T_EMIT2((opc & 0170000) | sz | (mrg_1&7)<<9 | 0400 | mrg_2, 0, 0, 0);
	ea_2(sz, bits);
	return(1);
}

cmp(sz)
{
	register opc;

	if ((mrg_1&070) == 030 && (mrg_2&070) == 030) {
		T_EMIT2(0130410 | sz | (mrg_1&7) | (mrg_2&7)<<9, 0, 0, 0);
		return;
	}
	if (mrg_1 == 074 && (mrg_2 & 070) != 010) {
		if (mrg_2==074)
			badoperand();
		T_EMIT2(06000 | sz | mrg_2, 0, 0, 0);
		ea_1(sz, 0);
		ea_2(sz, DTA);
		return;
	}
	if (mrg_2 < 020) {
		if (mrg_2 >= 010) {
			checksize(sz, 2|4);
			opc = 0130300 | sz<<1;
			mrg_2 &= 7;
		} else
			opc = 0130000 | sz;
		T_EMIT2(opc | mrg_2<<9 | mrg_1, 0, 0, 0);
		ea_1(sz, 0);
		return;
	}
	badoperand();
}

link_instr(sz, areg)
{
	if (sz == SIZE_NON) {
		if (bd_2.typ == S_ABS && fitw(bd_2.val))
			sz = SIZE_W;
		else
			sz = SIZE_L;
	}
	checksize(sz, 2|4);
	if (sz == SIZE_W)
		T_EMIT2(047120 | areg, 0, 0, 0);
	else /* sz == SIZE_L */
		T_EMIT2(044010 | areg, 0, 0, 0);
	ea_2(sz, 0);
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
		small(bd_1.typ==S_ABS && fitb(bd_1.val), 4)
	) {
		T_EMIT2(070000 | mrg_2<<9 | lowb(bd_1.val), 0, 0, 0);
		return;
	}
	switch (sz) {
	case SIZE_B:	opc = 010000; break;
	case SIZE_W:	opc = 030000; break;
	case SIZE_L:	opc = 020000; break;
	}
	T_EMIT2(opc | mrg_1 | (mrg_2&7)<<9 | (mrg_2&070)<<3, 0, 0, 0);
	ea_1(sz, 0);
	ea_2(sz, ALT);
}

move_special(sz)
{
	if (mrg_2 >= 076) {
		if (sz != SIZE_NON)
			checksize(sz, 2);
		T_EMIT2(042300 | (mrg_2==076?0:01000) | mrg_1, 0, 0, 0);
		ea_1(SIZE_W, DTA);
		return;
	}
	if (mrg_1 >= 076) {
		if (sz != SIZE_NON)
			checksize(sz, 2);
		T_EMIT2(040300 | (mrg_1==076?01000:0) | mrg_2, 0, 0, 0);
		ea_2(SIZE_W, DTA|ALT);
		return;
	}
	if (sz != SIZE_NON)
		checksize(sz, 4);
	if (mrg_1==075 && (mrg_2&070)==010) {
		T_EMIT2(047150 | (mrg_2&7), 0, 0, 0);
		return;
	}
	if (mrg_2==075 && (mrg_1&070)==010) {
		T_EMIT2(047140 | (mrg_1&7), 0, 0, 0);
		return;
	}
	badoperand();
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
	T_EMIT2(044200 | dr<<10 | (sz & 0200) >> 1 | mrg_2, 0, 0, 0);
	T_EMIT2(regs, 0, 0, 0);
	i = CTR;
	if (dr == 0 && (mrg_2&070) == 040)
		i = MEM;
	if (dr != 0 && (mrg_2&070) == 030)
		i = MEM;
	if (dr == 0)
		i |= ALT;
	ea_2(sz, i);
}

reverse(regs, max)
	register int regs;
{
	register int r, i;

	r = regs; regs = 0;
	for (i = max; i > 0; i--) {
		regs <<= 1;
		if (r & 1)
			regs++;
		r >>= 1;
	}
	return regs;
}

movep(sz)
{
	checksize(sz, 2|4);
	if (mrg_1<010 && (mrg_2&070)==050) {
		T_EMIT2(0610 | (sz & 0200)>>1 | mrg_1<<9 | (mrg_2&7), 0, 0, 0);
		ea_2(sz, 0);
		return;
	}
	if (mrg_2<010 && (mrg_1&070)==050) {
		T_EMIT2(0410 | (sz & 0200)>>1 | mrg_2<<9 | (mrg_1&7), 0, 0, 0);
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
	sm = fitw(exp.val);
	if ((exp.typ & ~S_DOT) != DOTTYP)
		sm = 0;
	if (small(sm,2)) {
		if (small(fitb(exp.val),2)) {
			if (exp.val == 0)
				T_EMIT2(047161, 0, 0, 0);	/* NOP */
			else if (exp.val == -1) {
				T_EMIT2(047161, 0, 0, 0);
				serror("bad branch offset");
			} else
				T_EMIT2(opc | lowb(exp.val), 0, 0, 0);
		} else {
			T_EMIT2(opc, 0, 0, 0);
			T_EMIT2(loww(exp.val), 0, 0, 0);
		}
		return;
	}
	sm = small(0,2);	/* dummy call; two calls to small per branch */
	T_EMIT2(opc | 0377, 0, 0, 0); /* 4 byte offset */
	T_EMIT4(exp.val, exp.typ, RELPC|RELO4, relonami);
}

cpbcc(opc, exp)
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
		T_EMIT2(opc, 0, 0, 0);
		T_EMIT2(loww(exp.val), 0, 0, 0);
		return;
	}
	
	T_EMIT2(opc | 0100, 0, 0, 0); /* 4 byte offset */
		/* NB: no coprocessor defined extension words are emitted */
	T_EMIT4(exp.val, exp.typ, RELPC|RELO4, relonami);
}

ea7071(sz)
{
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
		/* If this absolute address is in program space, and if we
		 * can assume that the only references to program space are made
		 * by instructins like 'jsr', 'jmp', 'lea' and 'pea', it might
		 * be possible to use a (PC,d16) effective address mode instead
		 * of absolute long. This is done here. If this scheme is in
		 * some way undesirable (e.g. when references to program space
		 * are made by instructions with more than one opcode word or by
		 * second effective addresses in instructions), the rest
		 * of this routine can simply be removed and replaced by the
		 * next two lines (which of course are in comment brackets now).
	if (small(bd_2.typ == S_ABS && fitw(bd_2.val), 2))
		mrg_2 = 070;
		 */
	if (pass == PASS_1) {
			/* Reserve a bit in the bittable; in the following
			 * passes one call to small() will be done, but know yet
			 * which one, because bd_2.typ cannot be trusted yet.
			 */
		small(0, 2);
		return;
	}
	if ((bd_2.typ & ~S_DOT) == DOTTYP) {
			/* the "off" variable fixes the problem described above,
			 * e.g., when references to program space are made by
			 * instructions with more than one opcode word.
			 */
		int off = 2;

		switch(curr_instr) {
		case MOVEM:
		case FMOVE:
		case FMOVEM:
		case FDYADIC:
		case FMONADIC:
		case FSINCOS:
		case FSCC:
		case FTST:
		case DIVL:
		case OP_RANGE:
		case CALLM:
		case CAS:
		case CPSCC:
		case CPTRAPCC:
		case PFLUSH:
		case PTEST:
		case PMOVE:
		case PLOAD:
			off = 4;
			break;
		case MOVESP:
			if (curr_size != 0) off = 4;
			break;
		case DIVMUL:
			if (curr_size != SIZE_W) off = 4;
			break;
		}
		if (small(fitw(bd_2.val-(DOTVAL+off)), 2)) {
			bd_2.val -= (DOTVAL+off);
			mrg_2 = 072;
		}
	} else
		if (small(bd_2.typ == S_ABS && fitw(bd_2.val), 2))
			mrg_2 = 070;
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
		T_EMIT2(0170200|co_id|opc, 0, 0, 0);
		T_EMIT2(loww(exp.val), 0, 0, 0);
		return;
	}
	T_EMIT2(0170300|co_id|opc, 0, 0, 0); /* 4 byte offset */
	T_EMIT4(exp.val, exp.typ, RELPC|RELO4, relonami);
}
