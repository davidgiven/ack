/*
 * (c) copyright 1983 by the Vrije Universiteit, Amsterdam, The Netherlands.
 *
 *          This product is part of the Amsterdam Compiler Kit.
 *
 * Permission to use, sell, duplicate or disclose this software must be
 * obtained in writing. Requests for such permissions may be sent to
 *
 *      Dr. Andrew S. Tanenbaum
 *      Wiskundig Seminarium
 *      Vrije Universiteit
 *      Postbox 7161
 *      1007 MC Amsterdam
 *      The Netherlands
 *
 */

#ifndef NORCSID
static char rcsid[] = "$Header$";
#endif

/*
 * Encode to compact EM assembly language
 *
 * Author: Johan Stevenson, Vrije Universiteit, Amsterdam
 */

#include	<stdio.h>
#include	<ctype.h>
#include	<assert.h>
#include	<setjmp.h>
#include	<em_spec.h>
#include	<em_pseu.h>
#include	<em_flag.h>
#include	<em_ptyp.h>
#include	<em_mes.h>

#define put8(x)		putchar(x)

#define check(x)	if (!(x)) fail_check()

#define fit16i(x)	((x) >= 0xFFFF8000 && (x) <= 0x00007FFF)
#define	fit8u(x)	((x) >= 0 && (x) <= 0xFF)

#define	MAXSTR		256
#define	HSIZE		256
#define	EMPTY		(EOF-1)

/*
 * global variables
 */

int	opcode;
int	offtyp;
long	argval;
int	dlbval;
char	string[MAXSTR];
int	strsiz;

int	wsize;
int	psize;
int	lineno;
int	argnum;
int	errors;
char	*progname;
char	*filename = "INPUT";

long	wordmask[] = {	/* allowed bits in a word */
	0x00000000,
	0x000000FF,
	0x0000FFFF,
	0x00000000,
	0xFFFFFFFF
};

long	sizemask[] = {	/* allowed bits in multiples of 'wsize' */
	0x00000000,
	0x7FFFFFFF,
	0x7FFFFFFE,
	0x00000000,
	0x7FFFFFFC
};

int	peekc = EMPTY;
int	hashtab[HSIZE];
jmp_buf	recover;

/*
 * external tables
 */

extern	char	em_flag[];
extern	short	em_ptyp[];
extern	char	em_mnem[][4];
extern	char	em_pseu[][4];

int main(argc,argv) char **argv; {

	progname = argv[0];
	if (argc >= 2) {
		filename = argv[1];
		if (freopen(filename,"r",stdin) == NULL)
			fatal("can't open %s",filename);
	}
	if (argc >= 3)
		if (freopen(argv[2],"w",stdout) == NULL)
			fatal("can't create %s",argv[2]);
	init();
	put16(sp_magic);
	setjmp(recover);
	while (nextline())
		;
	return(errors ? -1 : 0);
}

/* ----- copy ----- */

int nextline() {
	register c,i;

	lineno++;
	argnum = 1;
	c = nextchar();
	if (c == EOF)
		return(0);
	if (isspace(c) && c != '\n') {
		c = nospace();
		if (isalpha(c)) {
			inmnem(c);
			if (opcode <= sp_lmnem)
				instr();
			else
				pseudo();
		} else
			peekc = c;
	} else if (c == '#') {
		line_line();
	} else {
		peekc = c;
		i = gettyp(sym_ptyp | ptyp(sp_cst2) | ptyp(sp_cend));
		switch (i) {
		case sp_cst2:
			i = (int) argval;
			if (i >= 0 && i < sp_nilb0)
				put8(i + sp_filb0);
			else
				putarg(sp_ilb2);
			break;
		case sp_dlb2:
		case sp_dnam:
			putarg(i);
			break;
		case sp_cend:
			break;
		}
	}
	if (nospace() != '\n')
		syntax("end of line expected");
	return(1);
}

instr() {
	register i,j,t;
	register long l;

	i = opcode;
	put8(i);
	i -= sp_fmnem;
	j = em_flag[i] & EM_PAR;
	if (j == PAR_NO)
		return;
	t = em_ptyp[j];
	if (j == PAR_B)
		t = ptyp(sp_ilb2);
	t = getarg(t);
	/*
	 * range checking
	 */
	switch (j) {
	case PAR_N:
		check(argval >= 0);
		break;
	case PAR_G:
		if (t != sp_cst2 && t != sp_cst4)
			break;
		check(argval >= 0);
		/* fall through */
	case PAR_L:
		l = argval >= 0 ? argval : -argval;
		check((l & ~wordmask[psize]) == 0);
		break;
	case PAR_W:
		if (t == sp_cend)
			break;
		check((argval & ~wordmask[wsize]) == 0);
		/* fall through */
	case PAR_S:
		check(argval != 0);
		/* fall through */
	case PAR_Z:
		check((argval & ~sizemask[wsize]) == 0);
		break;
	case PAR_O:
		check(argval != 0);
		check((argval & ~sizemask[wsize])==0 || (wsize % argval)==0);
		break;
	case PAR_B:
		t = sp_cst2;
		break;
	case PAR_R:
		check(argval >= 0 && argval <= 2);
		break;
	}
	putarg(t);
}

pseudo() {
	register i,t;

	i = opcode;
	put8(i);
	switch (i) {
	case ps_bss:
	case ps_hol:
		putarg(getarg(cst_ptyp));
		putarg(getarg(val_ptyp));
		putarg(getarg(ptyp(sp_cst2)));
		check(argval==0 || argval==1);
		break;
	case ps_rom:
	case ps_con:
		putarg(getarg(val_ptyp));
		do
			putarg(t = getarg(any_ptyp));
		while (t != sp_cend);
		break;
	case ps_mes:
		putarg(getarg(ptyp(sp_cst2)));
		if (argval == ms_emx) {
			putarg(getarg(ptyp(sp_cst2)));
			check(argval > 0 && argval <= 4);
			wsize = (int) argval;
			putarg(getarg(ptyp(sp_cst2)));
			check(argval > 0 && argval <= 4);
			psize = (int) argval;
		}
		do
			putarg(t = getarg(any_ptyp));
		while (t != sp_cend);
		break;
	case ps_exa:
	case ps_ina:
		putarg(getarg(sym_ptyp));
		break;
	case ps_exp:
	case ps_inp:
		putarg(getarg(ptyp(sp_pnam)));
		break;
	case ps_exc:
		putarg(getarg(ptyp(sp_cst2)));
		putarg(getarg(ptyp(sp_cst2)));
		break;
	case ps_pro:
		putarg(getarg(ptyp(sp_pnam)));
		putarg(getarg(cst_ptyp|ptyp(sp_cend)));
		break;
	case ps_end:
		putarg(getarg(cst_ptyp|ptyp(sp_cend)));
		break;
	default:
		syntax("bad pseudo %d",i);
	}
}

/* ----- input ----- */

int getarg(typset) {
	register c;

	if (argnum != 1) {
		c = nospace();
		if (c != ',') {
			if (c != '\n')
				syntax("comma expected");
			peekc = c;
		}
	}
	argnum++;
	return(gettyp(typset));
}

int gettyp(typset) {
	register c,t,sp;

	c = nospace();
	if (c == '\n') {
		peekc = c;
		sp = sp_cend;
	} else if (isdigit(c) || c == '+' || c == '-' || c == '(') {
		sp = inexpr1(c);
		if (sp == sp_cst4 && fit16i(argval))
			sp = sp_cst2;
	} else if (isalpha(c)) {
		inname(c);
		sp = offsetted(sp_dnam);
	} else if (c == '.') {
		in15u();
		dlbval = (int) argval;
		sp = offsetted(sp_dlb2);
	} else if (c == '*') {
		in15u();
		sp = sp_ilb2;
	} else if (c == '$') {
		inname(nextchar());
		sp = sp_pnam;
	} else if (c == '"' || c == '\'') {
		sp = instring(c);
	} else if (c == '?') {
		sp = sp_cend;
	} else
		syntax("operand expected");
	t = sp - sp_fspec;
	assert(t >= 0 && t < 16);
	t = 1 << t;
	if ((typset & t) == 0)
		error("bad argument type %d",sp);
	return(sp);
}

int offsetted(sp) {
	register c;

	c = nospace();
	if (c == '+' || c == '-') {
		gettyp(cst_ptyp);
		if (c == '-')
			argval = -argval;
		offtyp = sp;
		return(sp_doff);
	}
	peekc = c;
	return(sp);
}

inname(c) register c; {
	register char *p;

	if (isalpha(c) == 0)
		syntax("letter expected");
	p = string;
	do {
		if (p < &string[MAXSTR-1])
			*p++ = c;
		c = nextchar();
	} while (isalnum(c));
	peekc = c;
	*p = '\0';
	strsiz = p - string;
}

int inmnem(c) register c; {
	register unsigned h;
	register i;

	inname(c);
	h = hash(string);
	for (;;) {
		h++;
		h %= HSIZE;
		i = hashtab[h];
		if (i == 0)
			syntax("bad mnemonic");
		if (i <= sp_lmnem) {
			assert(i >= sp_fmnem);
			if (strcmp(string,em_mnem[i - sp_fmnem]) != 0)
				continue;
			return(opcode = i);
		}
		assert(i <= sp_lpseu && i >= sp_fpseu);
		if (strcmp(string,em_pseu[i - sp_fpseu]) != 0)
			continue;
		return(opcode = i);
	}
}

int inexpr1(c) register c; {
	long left;

	if ((c = inexpr2(c)) != sp_cst4)
		return(c);
	for (;;) {
		c = nospace();
		if (c != '+' && c != '-') {
			peekc = c;
			break;
		}
		left = argval;
		if (inexpr2(nospace()) != sp_cst4)
			syntax("term expected");
		if (c == '+')
			argval += left;
		else
			argval = left - argval;
	}
	return(sp_cst4);
}

int inexpr2(c) register c; {
	long left;

	if ((c = inexpr3(c)) != sp_cst4)
		return(c);
	for (;;) {
		c = nospace();
		if (c != '*' && c != '/' && c != '%') {
			peekc = c;
			break;
		}
		left = argval;
		if (inexpr3(nospace()) != sp_cst4)
			syntax("factor expected");
		if (c == '*')
			argval *= left;
		else if (c == '/')
			argval = left / argval;
		else
			argval = left % argval;
	}
	return(sp_cst4);
}

inexpr3(c) register c; {

	if (c == '(') {
		if (inexpr1(nospace()) != sp_cst4)
			syntax("expression expected");
		if (nospace() != ')')
			syntax("')' expected");
		return(sp_cst4);
	}
	return(innumber(c));
}

int innumber(c) register c; {
	register char *p;
	register n;
	int expsign;
	static char numstr[MAXSTR];
	long atol();

	p = numstr;
	expsign = 0;
	if (c == '+' || c == '-') {
		if (c == '-')
			*p++ = c;
		c = nextchar();
	}
	if (isdigit(c) == 0)
		syntax("digit expected");
	n = sp_cst4;
	for (;;) {
		if (p >= &numstr[MAXSTR-1])
			fatal("number too long");
		*p++ = c;
		c = nextchar();
		if (c == '.' || c == 'e' || c == 'E') {
			expsign = c != '.';
			n = sp_fcon;
			continue;
		}
		if (expsign) {
			expsign = 0;
			if (c == '+' || c == '-')
				continue;
		}
		if (isdigit(c) == 0)
			break;
	}
	peekc = c;
	*p = '\0';
	c = nospace();
	if (n == sp_fcon && c != 'F')
		syntax("'F' expected");
	if (c == 'I' || c == 'U' || c == 'F')
		return(incon(numstr,c));
	peekc = c;
	argval = atol(numstr);
	return(sp_cst4);
}

in15u() {

	if (innumber(nextchar()) != sp_cst4)
		syntax("integer expected");
	check((argval & ~077777) == 0);
}

int incon(p,c) register char *p; {
	register char *q;

	q = string;
	while (*q++ = *p++)
		;
	strsiz = q - string - 1;
	gettyp(cst_ptyp);
	return(c == 'I' ? sp_icon : (c == 'U' ? sp_ucon : sp_fcon));
}

int instring(termc) {
	register char *p;
	register c;

	p = string;
	for (;;) {
		c = nextchar();
		if (c == '\n' || c == EOF) {
			peekc = c;
			syntax("non-terminated string");
		}
		if (c == termc) {
			if (termc == '"')
				*p++ = '\0';
			break;
		}
		if (c == '\\')
			c = inescape();
		if (p >= &string[MAXSTR-1])
			fatal("string too long");
		*p++ = c;
	}
	strsiz = p - string;
	return(sp_scon);
}

int inescape() {
	register c,j,r;

	c = nextchar();
	if (c >= '0' && c <= '7') {
		r = c - '0';
		for (j = 0; j < 2; j++) {
			c = nextchar();
			if (c < '0' || c > '7') {
				peekc = c;
				return(r);
			}
			r <<= 3;
			r += (c - '0');
		}
		return(r);
	}
	switch (c) {
	case 'b':	return('\b');
	case 'f':	return('\f');
	case 'n':	return('\n');
	case 'r':	return('\r');
	case 't':	return('\t');
	}
	return(c);
}

int nospace() {
	register c;

	do
		c = nextchar();
	while (isspace(c) && c != '\n');
	if (c == ';')
		do
			c = nextchar();
		while (c != '\n' && c != EOF);
	return(c);
}

int nextchar() {
	register c;

	if (peekc != EMPTY) {
		c = peekc;
		peekc = EMPTY;
		return(c);
	}
	c = getchar();
	if (isascii(c) == 0 && c != EOF)
		fatal("non-ascii char");
	return(c);
}

line_line() {
	register char *p,*q;
	static char filebuff[MAXSTR+1];

	gettyp(ptyp(sp_cst2));
	lineno = (int) (argval-1);
	gettyp(ptyp(sp_scon));
	p = string;
	q = filebuff;
	while (--strsiz >= 0)
		*q++ = *p++;
	*q = '\0';
	filename = filebuff;
}

init() {
	register i;

	for (i = sp_fmnem; i <= sp_lmnem; i++)
		pre_hash(i,em_mnem[i - sp_fmnem]);
	for (i = sp_fpseu; i <= sp_lpseu; i++)
		pre_hash(i,em_pseu[i - sp_fpseu]);
	/* treat '_' as letter */
	/* In System III the array is called _ctype[] without the trailing '_' */
	(_ctype_+1)['_'] = (_ctype_+1)['a'];
}

pre_hash(i,s) char *s; {
	register unsigned h;

	assert(i != 0);
	h = hash(s);
	for (;;) {
		h++;
		h %= HSIZE;
		if (hashtab[h] == 0) {
			hashtab[h] = i;
			return;
		}
	}
}

int hash(s) register char *s; {
	register h;

	h = 0;
	while (*s) {
		h <<= 1;
		h += *s++;
	}
	return(h);
}

/* ----- output ----- */

putarg(sp) register sp; {
	register i;

	switch (sp) {
	case sp_ilb2:
		i = (int) argval;
		if (fit8u(i)) {
			put8(sp_ilb1);
			put8(i);
			break;
		}
		put8(sp);
		put16(i);
		break;
	case sp_dlb2:
		i = dlbval;
		if (fit8u(i)) {
			put8(sp_dlb1);
			put8(i);
			break;
		}
		put8(sp);
		put16(i);
		break;
	case sp_cst2:
	case sp_cst4:
		if (fit16i(argval) == 0) {
			put8(sp_cst4);
			put32(argval);
			break;
		}
		i = (int) argval;
		if (i >= -sp_zcst0 && i < sp_ncst0 - sp_zcst0) {
			put8(i + sp_zcst0 + sp_fcst0);
			break;
		}
		put8(sp_cst2);
		put16(i);
		break;
	case sp_doff:
		put8(sp);
		putarg(offtyp);
		putarg(sp_cst4);
		break;
	case sp_dnam:
	case sp_pnam:
	case sp_scon:
		put8(sp);
		putstr();
		break;
	case sp_icon:
	case sp_ucon:
	case sp_fcon:
		put8(sp);
		putarg(sp_cst4);
		putstr();
		break;
	case sp_cend:
		put8(sp);
		break;
	}
}

putstr() {
	register char *p;
	long consiz;

	consiz = argval;
	argval = strsiz;
	putarg(sp_cst4);
	argval = consiz;
	p = string;
	while (--strsiz >= 0)
		put8(*p++);
}

put16(w) int w; {

	put8(w);
	put8(w >> 8);
}

put32(f) long f; {

	put16((int) f);
	put16((int)(f >> 16));
}

/* ----- error handling ----- */

fail_check() {
	error("argument range error");
}

/* VARARGS */
error(s,a1,a2,a3,a4) char *s; {
	fprintf(stderr,"%s: line %d: ", filename, lineno);
	fprintf(stderr,s,a1,a2,a3,a4);
	fprintf(stderr,"\n");
	errors++;
}

/* VARARGS */
fatal(s,a1,a2,a3,a4) char *s; {
	error(s,a1,a2,a3,a4);
	exit(-1);
}

/* VARARGS */
syntax(s,a1,a2,a3,a4) char *s; {
	register c;

	error(s,a1,a2,a3,a4);
	do
		c = nextchar();
	while (c != '\n' && c != EOF);
	longjmp(recover);
}
