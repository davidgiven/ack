/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
#define RCSID5 "$Id$"

/*
 * Motorola 6805 special routines
 */

/* VARARGS2 */
branch(opc,exp,cell) register opc; expr_t cell; expr_t exp; {
	register sm, dist;
	int saving;

	dist = exp.val - (DOTVAL + 2);
	if((opc & 0xf0) == 0) dist -= 1;  /* bitbranch */
	if (pass == PASS_2 && dist > 0 && !(exp.typ & S_DOT))
		dist -= DOTGAIN;
	sm = fitj(dist);
	if ((exp.typ & ~S_DOT) != DOTTYP)
		sm = 0;
	if (opc == 0x20 || opc == 0xAD)
		saving = 1;
	else
		saving = 3;
	if (small(sm,saving)) {
		emit1(opc);
		if((opc & 0xF0) == 0)	/* bit branch */
			emit1(cell.val);
#ifdef RELOCATION
		newrelo(exp.typ, RELPC|RELO1);
#endif
		emit1(dist);
	} else {
		if (opc == 0xAD)		/* bsr */
			emit1(0xBD);		/* jsr */
		else {
			if (opc != 0x20) {	/* bra */

					/* reverse condition : */

				emit1(opc ^ 1);
				if((opc & 0xF0) == 0)  /* bitbranch */
					emit1(cell.val);
				emit1(3);
			}
			emit1(0xCC);		/* jmp */
		}
#ifdef RELOCATION
		newrelo(exp.typ, RELPC|RELO2|RELBR);
#endif
		emit2(exp.val);
	}
}
