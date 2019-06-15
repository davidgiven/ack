/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
#define RCSID5 "$Id$"

/*
 * Motorola 6800 special routines
 */

void branch(register int opc,expr_t exp)
{
	register int sm, dist;
	int saving;

	dist = exp.val - (DOTVAL + 2);
	if (pass == PASS_2 && dist > 0 && !(exp.typ & S_DOT))
		dist -= DOTGAIN;
	sm = fitb(dist);
	if ((exp.typ & ~S_DOT) != DOTTYP)
		sm = 0;
	if (opc == 0x8D || opc == 0x20)
		saving = 1;
	else
		saving = 3;
	if (small(sm,saving)) {
		emit1(opc);
		emit1(dist);
	} else {
		if (opc == 0x8D)		/* bsr */
			emit1(0xBD);		/* jsr */
		else {
			if (opc != 0x20) {	/* bra */

					/* reverse condition : */

				emit1(opc ^ 1);
				emit1(3);
			}
			emit1(0x7E);		/* jmp */
		}
#ifdef RELOCATION
		newrelo(exp.typ, RELO2 | RELBR);
#endif
		emit2(exp.val);
	}
}
