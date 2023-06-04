/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */

/*
 * Zilog Z80 special routines
 */

void xyreg(int r, int byte)
{

	if (r == IX)
		emit1(0xdd);
	else if (r == IY)
		emit1(0xfd);
	else if (r != HL)
		serror("register error");
	emit1(byte);
}

void xymem(int r, int byte)
{
	xyreg(r, byte);
	if (r != HL)
	{
#ifdef RELOCATION
		RELOMOVE(relonami, rel_ind);
		newrelo(exp_ind.typ, RELO1);
#endif
		emit1(exp_ind.val);
	}
}

void xylea(int r1, int r2)
{
	if ((r2 != IX) && (r2 != IY))
		serror("can't use this as index register");

	emit1(0xed);
	switch (r1)
	{
		case SP:
			emit1((r2 - 1) | 0x60);
			break;
			
		case HL:
		case BC:
		case DE:
			emit1((r1 << 4) | (r2 & 3));
			break;

		case IX:
			emit1((r2 == IX) ? 0x32 : 0x54);
			break;

		case IY:
			emit1((r2 == IX) ? 0x55 : 0x33);
			break;
	}

#ifdef RELOCATION
	RELOMOVE(relonami, rel_ind);
	newrelo(exp_ind.typ, RELO1);
#endif
	emit1(exp_ind.val);
}

void branch(register int opc, expr_t exp)
{
	register int sm, dist;

	dist = exp.val - (DOTVAL + 2);
	if (pass == PASS_2 && dist > 0 && !(exp.typ & S_DOT))
		dist -= DOTGAIN;
	sm = fitb(dist);
	if ((exp.typ & ~S_DOT) != DOTTYP)
		sm = 0;
	if (opc == 020)
	{ /* DJNZ must be short */
		fit(sm);
		sm = 1;
	}
	else if ((sm = small(sm, 1)) == 0)
	{
		if (opc == 030) /* 'JR exp' */
			opc = 0303; /* replace by 'JP exp' */
		else /* 'JR CC,exp' */
			opc ^= 0342; /* replace by 'JP CC,exp' */
	}
	emit1(opc);
	if (sm == 0)
	{
#ifdef RELOCATION
		newrelo(exp.typ, RELO2);
#endif
		emit2(exp.val);
	}
	else
	{
		emit1(lowb(dist));
	}
}
