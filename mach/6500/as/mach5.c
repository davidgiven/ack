/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
#define RCSID5 "$Id$"

/*
 * Mostek 6500 special routines.
 */

branch(opc, exp)
register int	opc;
expr_t		exp;
{
	register int	dist;

	dist = exp.val - (DOTVAL + 2);
	if (pass == PASS_2 && dist > 0 && !(exp.typ & S_DOT))
		dist -= DOTGAIN;
	if (small(fitb(dist) && (exp.typ & ~S_DOT) == DOTTYP, 3)) {
		emit1(opc); emit1(dist);
	} else {
		emit1(opc^0x20); emit1(0x03);	/* Skip over ... */
		emit1(0x4C);			/* ... far jump. */
#ifdef RELOCATION
		newrelo(exp.typ, RELO2);
#endif
		emit2(exp.val);
	}
}

code(exp, opc1, opc2)
expr_t		exp;
register int	opc1, opc2;
{
	if (small((exp.typ & S_TYP) == S_ABS && fits_zeropage(exp.val), 1)) {
		emit1(opc1); emit1(exp.val);
	} else {
		emit1(opc2);
#ifdef RELOCATION
		newrelo(exp.typ, RELO2);
#endif
		emit2(exp.val);
	}
}
