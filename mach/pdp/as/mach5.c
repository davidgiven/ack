#define RCSID5 "$Id$"

/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 *
 */

op1(mode)	{
	int relpc = 0;
	if (im1flag)	{
		if (mode == 067 || mode == 077) {
			exp_1.val = adjust(exp_1);
			relpc = RELPC;
		}
#ifdef RELOCATION
		RELOMOVE(relonami, rel_1);
		if (rflag != 0 && PASS_RELO)
			newrelo(exp_1.typ, RELO2|relpc);
#endif
		emit2(exp_1.val);
		im1flag = 0;
	}
}

op2(mode)	{
	int relpc = 0;
	if (im2flag)	{
		if (mode == 067 || mode == 077) {
			relpc = RELPC;
			exp_2.val = adjust(exp_2);
		}
#ifdef RELOCATION
		RELOMOVE(relonami, rel_2);
		if (rflag != 0 && PASS_RELO)
			newrelo(exp_2.typ, RELO2|relpc);
#endif
		emit2(exp_2.val);
		im2flag = 0;
	}
}

branch(opc,exp) expr_t exp; {
	register eval;
	register sm;

	eval = adjust(exp) >> 1;
	sm = fitb(eval);
	if ((exp.typ & ~S_DOT) != DOTTYP && pass >= PASS_2) sm = 0;
	if (!sm && pass >= PASS_2) {
		serror("label too far");
	}
	emit2(opc | lowb(eval));
}

ejump(opc, exp) expr_t exp; {
	register sm,eval;
	int gain;

# ifdef THREE_PASS
	eval = adjust(exp) >> 1;
	sm = fitb(eval);
	if ((exp.typ & ~S_DOT) != DOTTYP)	{
		sm = 0;
	}
	gain = (opc == OPBRA ? 2 : 4);
	if (small(sm,gain))	{
		emit2( opc | lowb(eval));
	}
	else	{
# endif
		if (opc != OPBRA)	{
			emit2((opc^0400) | 02);
		}

		exp_1 = exp;
		im1flag = 1;
		emit2(0100|067);
		op1(067);
# ifdef THREE_PASS
	}
# endif
}

sob(reg, exp) expr_t exp; {
	if ((exp.typ & ~S_DOT) != DOTTYP)	{
		serror("error in sob-label");
	}
	exp.val = ( - adjust(exp) ) >> 1;
	fit(fit6(exp.val));
	emit2( OPSOB | (reg << 6) | exp.val);
}

jump(opc,opr)	{
	register val;

# ifdef THREE_PASS
	if (opr==067) {
		register sm = 0;

		val = adjust(exp_1) >> 1;
		if ( fitb(val) && (exp_1.typ & ~S_DOT) == DOTTYP) {
			sm = 1;
		}
		if (small(sm,2)) {
			emit2(OPBRA | lowb(val));
			im1flag = 0;
			return(0);
		}
	}
# endif
	emit2(opc | opr);
	op1(opr);
}

valu_t adjust(exp) expr_t exp; {
	valu_t val;

	val = exp.val - DOTVAL - 2;
# ifdef THREE_PASS
	if (pass == PASS_2 && val > 0) val -= DOTGAIN;
# endif
	return(val);
}
