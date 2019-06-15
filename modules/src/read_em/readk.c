/* $Id$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/*	This file must be included in the file "read_emk.c".
	It takes care of the reading of compact EM code.
*/

#include <ctype.h>

/* get16, get32: read a signed constant
*/
PRIVATE int get16(void)
{
	register int l_byte, h_byte;

	l_byte = getbyte();
	h_byte = getbyte();
	if (h_byte >= 128) h_byte -= 256;
	return l_byte | (h_byte << 8);
}

PRIVATE arith get32(void)
{
	register arith l;
	register int h_byte;

	l = getbyte();
	l |= ((unsigned) getbyte() << 8);
	l |= ((arith) getbyte() << 16);
	h_byte = getbyte();
	if (h_byte >= 128) h_byte -= 256;
	return l | ((arith) h_byte << 24);
}

PRIVATE struct string *getstring();

/* getarg : read an argument of any type, and check it against "typset"
   if neccesary. Put result in "ap".
*/
PRIVATE void getarg(int typset, register struct e_arg *ap)
{
	register int i = getbyte();
#ifdef CHECKING
	int argtyp;
#endif /* CHECKING */

	ap->ema_argtype = 0;
	switch(i) {
	default:
		if (i < sp_fcst0+sp_ncst0 && i >= sp_fcst0) { /* A cst */
			ap->ema_cst = i - sp_zcst0;
			ap->ema_argtype = cst_ptyp;
			i = sp_cst2;
		}
		break;

	case sp_dlb1:	/* Numeric data label encoded in one byte */
		ap->ema_dlb = getbyte();
		ap->ema_szoroff = 0;
		ap->ema_argtype = nof_ptyp;
		break;

	case sp_dlb2:	/* Numeric data label encoded in two bytes */
		ap->ema_dlb = get16();
		ap->ema_szoroff = 0;
		ap->ema_argtype = nof_ptyp;
#ifdef CHECKING
		if (ap->ema_dlb > 32767 && !EM_error) {
			EM_error = "Illegal data label";
			break;
		}
#endif /* CHECKING */
		break;

	case sp_ilb1:	/* Instruction label encoded in one byte */
		ap->ema_ilb = getbyte();
		ap->ema_argtype = ilb_ptyp;
		break;

	case sp_ilb2:	/* Instruction label encoded in two bytes */
		ap->ema_ilb = get16();
		ap->ema_argtype = ilb_ptyp;
#ifdef CHECKING
		if (ap->ema_ilb > 32767 && !EM_error) {
			EM_error = "Illegal instruction label";
			break;
		}
#endif /* CHECKING */
		break;

	case sp_cst2:	/* A cst encoded in two bytes */
		ap->ema_cst = get16();
		ap->ema_argtype = cst_ptyp;
		break;

	case sp_cst4:	/* A cst encoded in four bytes */
		ap->ema_cst = get32();
		ap->ema_argtype = cst_ptyp;
		break;

	case sp_pnam:	/* A procedure name */
	{
		register struct string *p;

		p = getstring(1);
		ap->ema_pnam = p->str;
		ap->ema_argtype = pro_ptyp;
		break;
	}

	case sp_dnam:	/* A Non-numeric data label */
	{
		register struct string *p;

		p = getstring(1);
		ap->ema_dnam = p->str;
		ap->ema_szoroff = 0;
		ap->ema_argtype = sof_ptyp;
		break;
	}

	case sp_doff:	/* A data label + offset */
	{
		struct e_arg dummy;

		getarg(lab_ptyp, ap);
		getarg(cst_ptyp, &dummy);
		ap->ema_szoroff = dummy.ema_cst;
		break;
	}

	case sp_icon:	/* An integer constant */
	case sp_ucon:	/* An unsigned constant */
	case sp_fcon:	/* A floating constant */
	{
		register struct string *p;

		getarg(cst_ptyp, ap);
		ap->ema_szoroff = ap->ema_cst;
		p = getstring(0);
		ap->ema_argtype = ptyp(i);
		ap->ema_string = p->str;
		break;
	}

	case sp_scon:	/* A string constant */
	{
		register struct string *p;

		p = getstring(0);
		ap->ema_argtype = str_ptyp;
		ap->ema_string = p->str;
		ap->ema_szoroff = p->length;
		break;
	}
	}
#ifdef CHECKING
	argtyp = i;
	if (EM_error) return;

	if (i == EOF) {
		xfatal("Unexpected EOF");
		return;
	}

	if ((i -= sp_fspec) < 0 || i >= 16) {
		xerror("Illegal byte");
		return;
	}

	if ((typset & (1 << i)) == 0 && !EM_error) {
		EM_error = "Bad argument type";
	}
	if (argtyp == sp_cend) {
		ap->ema_argtype = 0;
	}
#else /* not CHECKING */
	if (i == sp_cend) {
		ap->ema_argtype = 0;
	}
#endif /* CHECKING */
}

#ifdef CHECKING
/* checkident: check that a string indeed represents an identifier
*/
PRIVATE int checkident(register struct string *s)
{
	register char *p;
	register int n;

	p = s->str;
	if (!isascii(*p) || (!isalpha(*p) && *p != '_')) {
		return 0;
	}
	p++;
	for (n = s->length; --n > 0; p++) {
		if (!isascii(*p) || (!isalnum(*p) && *p != '_')) {
			return 0;
		}
	}
	return 1;
}
#endif /* CHECKING */

/* getstring: read a string from the input
*/
/*ARGSUSED*/
PRIVATE struct string *getstring(int isident)
{
	register char *p;
	register int n;
	register struct string *s = &string;
	struct e_arg dummy;

	getarg(cst_ptyp, &dummy);
					/* Read length of string */
	n = dummy.ema_cst;
#ifdef CHECKING
	if (n < 0) {
		xerror("Negative length in string");
		s->length = 0;
		return s;
	}
#endif /* CHECKING */

	if (n > s->maxlen) {
		if (! s->maxlen) {
			s->str = Malloc(s->maxlen = 256);
		}
		else	s->str = Realloc(s->str, (s->maxlen = (n+255)&~255));
	}

	s->length = n;
	p = s->str;
	while (--n >= 0) {
		*p++ = getbyte();
	}
	*p++ = '\0';

#ifdef CHECKING
	if (isident) {
		if (!checkident(s) && !EM_error) {
			EM_error = "Illegal identifier";
		}
	}
#endif /* CHECKING */
	return s;
}

/* gethead: read the start of an EM-line
*/
PRIVATE void gethead(register struct e_instr *p)
{
	register int i;

	EM_lineno++;

	if ((i = getbyte()) < sp_fmnem+sp_nmnem && i >= sp_fmnem) {
		/* A mnemonic */
		p->em_type = EM_MNEM;
		p->em_opcode = i;
		return;
	}

	if (i < sp_fpseu+sp_npseu && i >= sp_fpseu) {	/* A pseudo */
		if (i == ps_mes) {
			p->em_type = EM_STARTMES;
		}
		else	p->em_type = EM_PSEU;
		p->em_opcode = i;
		return;
	}

	if (i < sp_filb0+sp_nilb0 && i >= sp_filb0) {	/* Instruction label */
		p->em_type = EM_DEFILB;
		p->em_argtype = ilb_ptyp;
		p->em_ilb = i - sp_filb0;
		return;
	}

	switch(i) {
	case sp_ilb1:	/* Instruction label */
		p->em_type = EM_DEFILB;
		p->em_argtype = ilb_ptyp;
		p->em_ilb = getbyte();
		break;

	case sp_ilb2:	/* Instruction label */
		p->em_type = EM_DEFILB;
		p->em_argtype = ilb_ptyp;
		p->em_ilb = get16();
#ifdef CHECKING
		if (p->em_ilb > 32767 && !EM_error) {
			EM_error = "Illegal instruction label definition";
		}
#endif /* CHECKING */
		break;

	case sp_dlb1:	/* Numeric data label */
		p->em_type = EM_DEFDLB;
		p->em_argtype = nof_ptyp;
		p->em_dlb = getbyte();
		break;

	case sp_dlb2:	/* Numeric data label */
		p->em_type = EM_DEFDLB;
		p->em_argtype = nof_ptyp;
		p->em_dlb = get16();
#ifdef CHECKING
		if (p->em_dlb > 32767 && !EM_error) {
			EM_error = "Illegal data label definition";
		}
#endif /* CHECKING */
		break;

	case sp_dnam:	/* Non-numeric data label */
	{
		struct string *s;

		p->em_type = EM_DEFDNAM;
		p->em_argtype = sof_ptyp;
		if (!(s = getstring(1))) {
			p->em_dnam = "";
		}
		else	p->em_dnam = s->str;
		break;
	}		

	case EOF:	/* End of file */
		p->em_type = EM_EOF;
		return;

	default:
		xerror("Unknown opcode");
		break;
	}

	return;
}
