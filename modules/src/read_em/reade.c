/* $Id$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/*	This file is ment to be included in the file read_em.c.
	It contains the part that takes care of the reading of human readable
	EM-code.
*/

#include <ctype.h>
#include <string.h>
#include <ack_string.h>

/* #define XXX_YYY	/* only for early debugging */

#ifdef XXX_YYY
#define out(str)	(sys_write(STDOUT, str, strlen(str)))
#else
#define out(s)
#endif

#define fit16i(x)	((x) >= -32768L && (x) <= 32767L)

#define HSIZE	256	/* Size of hashtable for mnemonics */

static int hashtab[HSIZE];	/* The hashtable for mnemonics */

static int argnum;		/* Number of arguments */

#define COMMENTSTARTER	';'

/* inithash, pre_hash, hash: Simple hashtable mechanism
*/
PRIVATE int
hash(s)
	register char *s;
{
	register int h = 0;

	while (*s) {
		h <<= 1;
		h += *s++;
	}
	return h;
}

PRIVATE void
pre_hash(i, s)
	char *s;
{
	register int h;

	assert(i != 0);
	h = hash(s);

	for (;;) {
		h++;
		if (h >= HSIZE) h %= HSIZE;
		if (hashtab[h] == 0) {
			hashtab[h] = i;
			return;
		}
	}
	/*NOTREACHED*/
}

extern char em_mnem[][4];
extern char em_pseu[][4];

PRIVATE void
inithash()
{
	register int i;

	/* Enter instructions ... */
	for (i = sp_fmnem; i <= sp_lmnem; i++) {
		pre_hash(i, em_mnem[i - sp_fmnem]);
	}

	/* and pseudos ... */
	for (i = sp_fpseu; i <= sp_lpseu; i++) {
		pre_hash(i, em_pseu[i - sp_fpseu]);
	}
}

/* nospace: skip until we find a non-space character. Also skip
	comments.
*/
PRIVATE int
nospace()
{
	register int c;

	do	c = getbyte();
	while (isspace(c) && c != '\n');

	if (c == COMMENTSTARTER) {
		do	c = getbyte();
		while (c != '\n' && c != EOF);
	}

	return c;
}

/* syntax: Put an error message in EM_error and skip to the end of the line
*/
PRIVATE void
syntax(s)
	char *s;
{
	register int c;

	xerror(s);
	state = 0;
	while ((c = getbyte()) != '\n' && c != EOF) /* nothing */ ;
	ungetbyte(c);
}

/* checkeol: check that we have a complete line (except maybe for spaces)
*/
PRIVATE void
checkeol()
{

	if (nospace() != '\n') {
		syntax("end of line expected");
		nospace();
	}
}

/* getescape: read a '\' escape sequence
*/
PRIVATE int
getescape()
{
	register int c, j, r;

	if ((c = getbyte()) >= '0' && c <= '7') { /* numeric escape */
		r = c - '0';

		for (j = 0; j < 2; j++) {
			if ((c = getbyte()) < '0' || c > '7') {
				ungetbyte(c);
				return r;
			}
			r <<= 3;
			r += c - '0';
		}

		return r;
	}

	switch(c) {
	case 'b':	return '\b';
	case 'f':	return '\f';
	case 'n':	return '\n';
	case 'r':	return '\r';
	case 't':	return '\t';
	}

	return c;
}

/* getname: Read a string of characters representing an identifier
*/
PRIVATE struct string *
getname()
{
	register char *p;
	register struct string *s;
	register int c;

	s = &string;
	p = s->str;
	if (!p) {
		s->maxlen = 256;
		s->str = p = Malloc(256);
	}
	c = getbyte();

	if (!(isalpha(c) || c == '_')) {
		ungetbyte(c);
		syntax("Letter expected");
		return s;
	}

	while (isalnum(c) || c == '_') {
		if (p >= &(s->str[s->maxlen])) {
			int df = p - s->str;
			s->str = Realloc(s->str, (s->maxlen += 256));
			p = s->str + df;
		}
		*p++ = c;
		c = getbyte();
	}

	ungetbyte(c);
	*p = '\0';
	s->length = p - s->str;
	return s;
}

/* getstring: read a string of characters between quotes
*/
PRIVATE struct string *
getstring()
{
	register char *p;
	struct string *s;
	register int c;
	static int termc;

	s = &string;
	p = s->str;
	if (!p) {
		s->maxlen = 256;
		s->str = p = Malloc(256);
	}

	termc = getbyte();
	/* assert(termc == '"' || termc == '\''); */
	/* This assertion does not work. Compiler error messages.
	   The trouble lies in the ", it terminates the string
	   created in the assertion macro
	*/

	for (;;) {
		if ((c = getbyte()) == '\n' || c == EOF) {
			ungetbyte(c);
			syntax("non-terminated string");
			break;
		}

		if (c == termc) {
			if (termc == '"') *p++ = '\0';
			break;
		}

		if (c == '\\') c = getescape();

		if (p >= &(s->str[s->maxlen])) {
			int df = p - s->str;
			s->str = Realloc(s->str, (s->maxlen += 256));
			p = s->str + df;
		}

		*p++ = c;	
	}
	*p = '\0';

	s->length = p - s->str;
	return s;
}

PRIVATE void gettyp();

PRIVATE int
offsetted(argtyp, ap)
	arith *ap;
{
	register int c;

	if ((c = nospace()) == '+' || c == '-') {
		struct e_arg dummy;

		gettyp(cst_ptyp, &dummy);
		if (c == '-') *ap = -(dummy.ema_cst);
		else *ap = dummy.ema_cst;
		return sp_doff;
	}
	else	*ap = 0;

	ungetbyte(c);
	return argtyp;
}

PRIVATE int
getnumber(c, ap)
	register int c;
	register struct e_arg *ap;
{
	char str[256 + 1];
	register char *p = str;
	int n;
	int expsign;

	ap->ema_argtype = cst_ptyp;
	expsign = 0;

	if (c == '+' || c == '-') {
		if (c == '-') *p++ = c;
		c = getbyte();
	}

	/* Soak up any whitespace (to allow "- 4" to be considered a number). */
	
	while (isspace(c))
		c = getbyte();
		
	if (! isdigit(c)) {
		ungetbyte(c);
		syntax("digit expected");
		return sp_cst4;
	}

	n = sp_cst4;

	for (;;) {
		if (p >= &(str[256])) {
			syntax("number too long");
			return sp_cst4;
		}

		*p++ = c;

		if ((c = getbyte()) == '.' || c == 'e' || c == 'E') {
			expsign = c != '.';
			n = sp_fcon;
			continue;
		}

		if (expsign) {
			expsign = 0;
			if (c == '+' || c == '-') continue;
		}

		if (! isdigit(c)) break;
	}

	ungetbyte(c);
	*p = '\0';
	c = nospace();

	if (n == sp_fcon && c != 'F') {
		ungetbyte(c);
		syntax("'F' expected");
		return n;
	}	

	if (c == 'I' || c == 'U' || c == 'F') {
		struct e_arg dummy;

		strcpy(string.str, str);
		ap->ema_string = string.str;
		gettyp(cst_ptyp, &dummy);
		ap->ema_szoroff = dummy.ema_cst;

		switch(c) {
		case 'I':
			ap->ema_argtype = ico_ptyp;
			return sp_icon;
		case 'U':
			ap->ema_argtype = uco_ptyp;
			return sp_ucon;
		case 'F':
			ap->ema_argtype = fco_ptyp;
			return sp_fcon;
		}
		assert(0);
	}

	ungetbyte(c);
	ap->ema_cst = (arith) str2long(str, 10);
	return sp_cst4;
}

PRIVATE int getexpr();

PRIVATE int
getfactor(c, ap)
	register int c;
	register struct e_arg *ap;
{
	if (c == '(') {
		if (getexpr(nospace(), ap) != sp_cst4) {
			syntax("expression expected");
		}
		else if ((c = nospace()) != ')') {
			ungetbyte(c);
			syntax("')' expected");
		}
		return sp_cst4;
	}
	return getnumber(c, ap);
}

PRIVATE int
getterm(c, ap) 
	register int c;
	register struct e_arg *ap;
{
	arith left;

	if ((c = getfactor(c, ap)) != sp_cst4) return c;

	for (;;) {
		if ((c = nospace()) != '*' && c != '/' && c != '%') {
			ungetbyte(c);
			break;
		}

		left = ap->ema_cst;
		if (getfactor(nospace(), ap) != sp_cst4) {
			syntax("factor expected");
			break;
		}

		if (c == '*') ap->ema_cst *= left;
		else if (c == '/') ap->ema_cst = left / ap->ema_cst;
		else	ap->ema_cst = left % ap->ema_cst;
	}
	return sp_cst4;
}

PRIVATE int
getexpr(c, ap)
	register int c;
	register struct e_arg *ap;
{
	arith left;

	if ((c = getterm(c, ap)) != sp_cst4) return c;

	for (;;) {
		if ((c = nospace()) != '+' && c != '-') {
			ungetbyte(c);
			break;
		}

		left = ap->ema_cst;
		if (getterm(nospace(), ap) != sp_cst4) {
			syntax("term expected");
			break;
		}

		if (c == '+') ap->ema_cst += left;
		else ap->ema_cst = left - ap->ema_cst;
	}
	return sp_cst4;
}

PRIVATE int
get15u()
{
	struct e_arg dummy;

	if (getnumber(getbyte(), &dummy) != sp_cst4) {
		syntax("integer expected");
	}
	else check((dummy.ema_cst & ~077777) == 0);
	return (int) (dummy.ema_cst);
}

PRIVATE void
gettyp(typset, ap)
	register struct e_arg *ap;
{
	register int c, t;
	register int argtyp;

	if ((c = nospace()) == '\n') {
		ungetbyte(c);
		out("newline\n");
		argtyp = sp_cend;
	}
	else if (isdigit(c) || c == '+' || c == '-' || c == '(') {
		out("expr\n");
		argtyp = getexpr(c, ap);
		if (argtyp == sp_cst4 && fit16i(ap->ema_cst)) argtyp = sp_cst2;
	}
	else if (isalpha(c) || c == '_') {
		out("name\n");
		ungetbyte(c);
		ap->ema_dnam = getname()->str;
		ap->ema_argtype = sof_ptyp;
		argtyp = offsetted(sp_dnam, &(ap->ema_szoroff));
	}
	else if (c == '.') {
		out(".label\n");
		ap->ema_dlb = get15u();
		ap->ema_argtype = nof_ptyp;
		argtyp = offsetted(sp_dlb2, &(ap->ema_szoroff));
	}
	else if (c == '*') {
		out("*label\n");
		ap->ema_ilb = get15u();
		ap->ema_argtype = ilb_ptyp;
		argtyp = sp_ilb2;
	}
	else if (c == '$') {
		out("$name\n");
		ap->ema_pnam = getname()->str;
		ap->ema_argtype = pro_ptyp;
		argtyp = sp_pnam;
	}
	else if (c == '"' || c == '\'') {
		register struct string *s;

		out("string\n");
		ungetbyte(c);
		s = getstring(0);
		ap->ema_string = s->str;
		ap->ema_szoroff = s->length;	
		ap->ema_argtype = str_ptyp;
		argtyp = sp_scon;
	}
	else if (c == '?') {
		out("?\n");
		argtyp = sp_cend;
		ap->ema_argtype = 0;
	}
	else {
		/* c != '\n', so "ungetbyte" not neccesary */
		syntax("operand expected");
		return;
	}

	t = argtyp - sp_fspec;
	assert(t >= 0 && t < 16);
	if ((typset & (1 << t)) == 0) {
		syntax("Bad argument type");
		return;
	}

	if (argtyp == sp_cend) {
		ap->ema_argtype = 0;
	}
}

PRIVATE void
getarg(typset, ap)
	struct e_arg *ap;
{
	register int c;

	if (argnum != 1) {
		if ((c = nospace()) != ',') {
			if (c != '\n') {
				syntax("comma expected");
				return;
			}
			ungetbyte(c);
		}
	}
	argnum++;
	gettyp(typset, ap);
}

/* getmnem: We found the start of either an instruction or a pseudo.
	get the rest of it
*/
PRIVATE void
getmnem(c, p)
	register struct e_instr *p;
{
	register int h;
	int i;
	register struct string *s;

	ungetbyte(c);
	s = getname();
	h = hash(s->str);

	for (;;) {
		h++;
		if (h >= HSIZE) h %= HSIZE;
		if ((i = hashtab[h]) == 0) {
			syntax("bad mnemonic");
			return;
		}
		else if (i <= sp_lmnem) {
			assert(i >= sp_fmnem);
			if (strcmp(s->str, em_mnem[i - sp_fmnem]) != 0) {
				continue;
			}
			p->em_type = EM_MNEM;
			p->em_opcode = i;
			break;
		}
		assert(i <= sp_lpseu && i >= sp_fpseu);
		if (strcmp(s->str, em_pseu[i - sp_fpseu]) != 0) {
			continue;
		}
		if (i == ps_mes) {
			p->em_type = EM_STARTMES;
			break;
		}
		p->em_opcode = i;
		p->em_type = EM_PSEU;
		break;
	}
}

PRIVATE void
line_line()
{
	static char filebuf[256 + 1];
	char *btscpy();
	struct e_arg dummy;

	gettyp(ptyp(sp_cst2), &dummy);
	EM_lineno = dummy.ema_cst;
	gettyp(str_ptyp, &dummy);
	btscpy(filebuf, dummy.ema_string, (int) dummy.ema_szoroff);
	EM_filename = filebuf;
}

PRIVATE void
getlabel(c, p)
	register struct e_instr *p;
{

	ungetbyte(c);
	gettyp(lab_ptyp|ptyp(sp_cst2), &(p->em_arg));
	switch(p->em_argtype) {
	case cst_ptyp:
		p->em_type = EM_DEFILB;
		p->em_argtype = ilb_ptyp;
		p->em_ilb = p->em_cst;
		break;
	case sof_ptyp:
		p->em_type = EM_DEFDNAM;
		break;
	case nof_ptyp:
		p->em_type = EM_DEFDLB;
		break;
	}
	checkeol();
}

PRIVATE void
gethead(p)
	register struct e_instr *p;
{
	register int c;

	argnum = 1;
	for (;;) {	
		EM_lineno++;
		c = getbyte();
		if (c == COMMENTSTARTER) {
			do	c = getbyte();
			while (c != '\n' && c != EOF);
		}
		if (c == EOF) {
			p->em_type = EM_EOF;
			return;
		}
		if (c == '\n') continue;
		if (isspace(c)) {
			c = nospace();
			if (isalpha(c) || c == '_') {
				getmnem(c, p);
				return;
			}
			ungetbyte(c);
		}
		else if (c == '#') line_line();
		else {
			getlabel(c, p);
			return;
		}
		checkeol();
		if (p->em_type == EM_ERROR || p->em_type == EM_FATAL) return;
	}
	/*NOTREACHED*/
}
