/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
#define RCSID5 "$Id$"

/*
 * Zilog Z80 special routines
 */

xyreg(r,byte) {

	if (r==IX)
		emit1(0335);
	else if (r==IY)
		emit1(0375);
	else if (r!=HL)
		serror("register error");
	emit1(byte);
}

xymem(r,byte) {

	xyreg(r,byte);
	if (r != HL) {
#ifdef RELOCATION
		RELOMOVE(relonami, rel_ind);
		newrelo(exp_ind.typ, RELO1);
#endif
		emit1(exp_ind.val);
	}
}

branch(opc,exp) register opc; expr_t exp; {
	register sm,dist;

	dist = exp.val - (DOTVAL + 2);
	if (pass == PASS_2 && dist > 0 && !(exp.typ & S_DOT))
		dist -= DOTGAIN;
	sm = fitb(dist);
	if ((exp.typ & ~S_DOT) != DOTTYP)
		sm = 0;
	if (opc == 020) {		/* DJNZ must be short */
		fit(sm);
		sm = 1;
	} else if ((sm = small(sm,1)) == 0) {
		if (opc == 030)		/* 'JR exp' */
			opc = 0303;	/* replace by 'JP exp' */
		else			/* 'JR CC,exp' */
			opc ^= 0342;	/* replace by 'JP CC,exp' */
	}
	emit1(opc);
	if (sm == 0) {
#ifdef RELOCATION
		newrelo(exp.typ, RELO2);
#endif
		emit2(exp.val);
	} else {
		emit1(lowb(dist));
	}
}
