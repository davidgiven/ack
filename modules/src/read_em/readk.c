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
PRIVATE int
get16()
{
	register int l_byte, h_byte;

	l_byte = getbyte();
	h_byte = getbyte();
	if (h_byte >= 128) h_byte -= 256;
	return l_byte | (h_byte << 8);
}

PRIVATE arith
get32()
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
   if neccesary. Return a pointer to the argument.
*/
PRIVATE struct e_args *
getarg(typset)
{
	register struct e_args *ap = argentry();
	register int i = getbyte();
#ifdef CHECKING
	int argtyp;
#endif CHECKING

	ap->em_next = 0;
	switch(i) {
	default:
		if (i < sp_fcst0+sp_ncst0 && i >= sp_fcst0) { /* A cst */
			ap->em_cst = i - sp_zcst0;
			ap->em_argtype = cst_ptyp;
			i = sp_cst2;
		}
		break;

	case sp_dlb1:	/* Numeric data label encoded in one byte */
		ap->em_dlb = getbyte();
		ap->em_noff = 0;
		ap->em_argtype = nof_ptyp;
		break;

	case sp_dlb2:	/* Numeric data label encoded in two bytes */
		ap->em_dlb = get16();
		ap->em_noff = 0;
		ap->em_argtype = nof_ptyp;
#ifdef CHECKING
		if (ap->em_dlb < 0) {
			xerror("Illegal data label");
			break;
		}
#endif CHECKING
		break;

	case sp_ilb1:	/* Instruction label encoded in one byte */
		ap->em_ilb = getbyte();
		ap->em_argtype = ilb_ptyp;
		break;

	case sp_ilb2:	/* Instruction label encoded in two bytes */
		ap->em_ilb = get16();
		ap->em_argtype = ilb_ptyp;
#ifdef CHECKING
		if (ap->em_ilb < 0) {
			xerror("Illegal instruction label");
			break;
		}
#endif CHECKING
		break;

	case sp_cst2:	/* A cst encoded in two bytes */
		ap->em_cst = get16();
		ap->em_argtype = cst_ptyp;
		break;

	case sp_cst4:	/* A cst encoded in four bytes */
		ap->em_cst = get32();
		ap->em_argtype = cst_ptyp;
		break;

	case sp_pnam:	/* A procedure name */
	{
		register struct string *p;

		p = getstring(1);
#ifdef CHECKING
		if (state & INSTRING) {
			xerror("Procedure name too long");
		}
#endif CHECKING
		ap->em_pnam = p->str;
		ap->em_argtype = pro_ptyp;
		break;
	}

	case sp_dnam:	/* A Non-numeric data label */
	{
		register struct string *p;

		p = getstring(1);
#ifdef CHECKING
		if (state & INSTRING) {
			xerror("Data label too long");
		}
#endif CHECKING
		ap->em_dnam = p->str;
		ap->em_soff = 0;
		ap->em_argtype = sof_ptyp;
		break;
	}

	case sp_doff:	/* A data label + offset */
	{
		register struct e_args *ap1;

		ap1 = getarg(lab_ptyp);
		*ap = *ap1;
		i_emargs--;
		ap1 = getarg(cst_ptyp);
		if (ap->em_argtype == sof_ptyp) {	/* non-numeric label */
			ap->em_soff = ap1->em_cst;
		}
		else	ap->em_noff = ap1->em_cst;
		i_emargs--;
		break;
	}

	case sp_icon:	/* An integer constant */
	case sp_ucon:	/* An unsigned constant */
	case sp_fcon:	/* A floating constant */
	{
		register struct string *p;

		ap->em_size = getarg(cst_ptyp)->em_cst;
		i_emargs--;
		p = getstring(0);
#ifdef CHECKING
		if (state & INSTRING) {
			xerror("Numeric constant too long");
		}
#endif CHECKING
		ap->em_argtype = ptyp(i);
		ap->em_str = p->str;
		break;
	}

	case sp_scon:	/* A string constant */
	{
		register struct string *p;

		p = getstring(0);
		ap->em_argtype = str_ptyp;
		ap->em_str = p->str;
		ap->em_size = p->length;
		break;
	}
	}
#ifdef CHECKING
	argtyp = i;
	if (EM_error) return 0;

	if (i == EOF) {
		xfatal("Unexpected EOF");
		return 0;
	}

	if ((i -= sp_fspec) < 0 || i >= 16) {
		xerror("Illegal byte");
		return 0;
	}

	if ((typset & (1 << i)) == 0) {
		xerror("Bad argument type");
	}
	if (argtyp == sp_cend) return 0;
#else not CHECKING
	if (i == sp_cend) return 0;
#endif CHECKING
	return ap;
}

#ifdef CHECKING
/* checkident: check that a string indeed represents an identifier
*/
PRIVATE int
checkident(s)
	register struct string *s;
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
#endif CHECKING

/* getstring: read a string from the input, but at most STRSIZ characters
	of it. The next characters will be read another time around
*/
/*ARGSUSED*/
PRIVATE struct string *
getstring(isident)
{
	register char *p;
	register int n;
	register struct string *s;

	if (!(state & INSTRING)) {	/* Not reading a string yet */
		struct e_args *ap = getarg(cst_ptyp);
					/* Read length of string */
		i_emargs--;
		strleft = ap->em_cst;
#ifdef CHECKING
		if (strleft < 0) {
			xerror("Negative length in string");
			return 0;
		}
#endif CHECKING
	}

	s = stringentry();

	if (strleft <= STRSIZ) {	/* Handle the whole string */
		n = strleft;
		state &= ~INSTRING;
	}
	else {				/* Handle STRSIZ characters of it, and
					   indicate that there is more
					*/
	 	n = STRSIZ;
		strleft -= STRSIZ;
		state |= INSTRING;
	}

	s->length = n;
	p = s->str;
	while (--n >= 0) {
		*p++ = getbyte();
	}
	*p++ = '\0';

#ifdef CHECKING
	if (isident) {
		if (!checkident(s)) {
			xerror("Illegal identifier");
		}
	}
#endif CHECKING
	return s;
}

/* gethead: read the start of an EM-line
*/
PRIVATE struct e_instr *
gethead()
{
	register int i;
	register struct e_instr *p = &emhead;

	EM_lineno++;

	if ((i = getbyte()) < sp_fmnem+sp_nmnem && i >= sp_fmnem) {
		/* A mnemonic */
		p->em_type = EM_MNEM;
		p->em_opcode = i;
		return p;
	}

	if (i < sp_fpseu+sp_npseu && i >= sp_fpseu) {	/* A pseudo */
		if (i == ps_mes) {
			p->em_type = EM_STARTMES;
		}
		else	p->em_type = EM_PSEU;
		p->em_opcode = i;
		return p;
	}

	if (i < sp_filb0+sp_nilb0 && i >= sp_filb0) {	/* Instruction label */
		p->em_type = EM_DEFILB;
		p->em_deflb = i - sp_filb0;
		return p;
	}

	switch(i) {
	case sp_ilb1:	/* Instruction label */
		p->em_type = EM_DEFILB;
		p->em_deflb = getbyte();
		break;

	case sp_ilb2:	/* Instruction label */
		p->em_type = EM_DEFILB;
		p->em_deflb = get16();
#ifdef CHECKING
		if (p->em_deflb < 0) {
			xerror("Illegal instruction label definition");
		}
#endif CHECKING
		break;

	case sp_dlb1:	/* Numeric data label */
		p->em_type = EM_DEFDLB;
		p->em_deflb = getbyte();
		break;

	case sp_dlb2:	/* Numeric data label */
		p->em_type = EM_DEFDLB;
		p->em_deflb = get16();
#ifdef CHECKING
		if (p->em_deflb < 0) {
			xerror("Illegal data label definition");
		}
#endif CHECKING
		break;

	case sp_dnam:	/* Non-numeric data label */
	{
		struct string *s;

		p->em_type = EM_DEFDNAM;
		if (!(s = getstring(1))) {
			p->em_type = EM_ERROR;
		}
		else	p->em_defdnam = s->str;
		break;
	}		

	case EOF:	/* End of file */
		return 0;

	default:
		xerror("Unknown opcode");
		break;
	}

	return p;
}
