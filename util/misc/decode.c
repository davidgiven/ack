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
 * Decode compact EM assembly language
 *
 * Author: Johan Stevenson, Vrije Universiteit, Amsterdam
 */

#include        <stdio.h>
#include        <assert.h>
#include        <ctype.h>
#include        <em_spec.h>
#include        <em_pseu.h>
#include        <em_flag.h>
#include        <em_ptyp.h>
#include        <em_mes.h>

#define get8()          ((unsigned)getchar())

#define check(x)        if (!(x)) fail_check()

#define MAXSTR          256

/*
 * global variables
 */

int     opcode;
int     offtyp;
long    argval;
int     dlbval;
char    string[MAXSTR];
int     strsiz;

int     wsize;
int     psize;
int     lineno;
int     argnum;
int     errors;
char    *progname;
char    *filename;

long    wordmask[] = {  /* allowed bits in a word */
	0x00000000,
	0x000000FF,
	0x0000FFFF,
	0x00000000,
	0xFFFFFFFF
};

long    sizemask[] = {  /* allowed bits in multiples of 'wsize' */
	0x00000000,
	0x7FFFFFFF,
	0x7FFFFFFE,
	0x00000000,
	0x7FFFFFFC
};

/*
 * external tables
 */

extern  char    em_flag[];
extern  short   em_ptyp[];
extern  char    em_mnem[][4];
extern  char    em_pseu[][4];

/*
 * routines
 */

int     get16();
long    get32();

main(argc,argv) char **argv; {

	progname = argv[0];
	if (argc >= 2) {
		filename = argv[1];
		if (freopen(argv[1],"r",stdin) == NULL)
			fatal("can't open %s",argv[1]);
	}
	if (argc >= 3)
		if (freopen(argv[2],"w",stdout) == NULL)
			fatal("can't create %s",argv[2]);
	if (get16() != sp_magic)
		fatal("bad magic word");
	/* In System III the array is called _ctype[] without the trailing '_' */
	(_ctype_+1)['_'] = (_ctype_+1)['a'];
	while (nextline())
		;
	return(errors ? -1 : 0);
}

/* ----- copy ----- */

int nextline() {
	register t;

	lineno++;
	argnum = 1;
	switch (t = table1()) {
	case EOF:
		return(0);
	case sp_fmnem:
		instr();
		break;
	case sp_fpseu:
		pseudo();
		break;
	case sp_ilb1:
	case sp_ilb2:
		argnum = 0;
		putarg(sp_cst2);
		break;
	case sp_dlb1:
	case sp_dlb2:
	case sp_dnam:
		argnum = 0;
		putarg(t);
		break;
	default:
		error("unknown opcode %d",t);
	}
	putchar('\n');
	return(1);
}

instr() {
	register i,j,t;
	register long l;

	i = opcode - sp_fmnem;
	printf(" %s",em_mnem[i]);
	j = em_flag[i] & EM_PAR;
	if (j == PAR_NO)
		return;
	t = em_ptyp[j];
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
		t = sp_ilb2;
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
	printf(" %s",em_pseu[i - sp_fpseu]);
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
		while ((t = getarg(any_ptyp)) != sp_cend)
			putarg(t);
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
		while ((t = getarg(any_ptyp)) != sp_cend)
			putarg(t);
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
		error("bad pseudo %d",i);
	}
}

/* ----- input ----- */

int getarg(typset) {
	register t,argtyp;

	argtyp = t = table2();
	if (t == EOF)
		fatal("unexpected EOF");
	t -= sp_fspec;
	assert(t >= 0 && t < 16);
	t = 1 << t;
	if ((typset & t) == 0)
		error("bad argument type %d",argtyp);
	return(argtyp);
}

int table1() {
	register i;

	i = get8();
	if (i < sp_fmnem+sp_nmnem && i >= sp_fmnem) {
		opcode = i;
		return(sp_fmnem);
	}
	if (i < sp_fpseu+sp_npseu && i >= sp_fpseu) {
		opcode = i;
		return(sp_fpseu);
	}
	if (i < sp_filb0+sp_nilb0 && i >= sp_filb0) {
		argval = i - sp_filb0;
		return(sp_ilb2);
	}
	return(table3(i));
}

int table2() {
	register i;

	i = get8();
	if (i < sp_fcst0+sp_ncst0 && i >= sp_fcst0) {
		argval = i - sp_zcst0;
		return(sp_cst2);
	}
	return(table3(i));
}

int table3(i) {
	long consiz;

	switch(i) {
	case sp_ilb1:
		argval = get8();
		break;
	case sp_dlb1:
		dlbval = get8();
		break;
	case sp_dlb2:
		dlbval = get16();
		if ( dlbval<0 ) {
			error("illegal data label .%d",dlbval);
			dlbval=0 ;
		}
		break;
	case sp_cst2:
		argval = get16();
		break;
	case sp_ilb2:
		argval = get16();
		if ( argval<0 ) {
			error("illegal instruction label %ld",argval);
			argval=0 ;
		}
		break;
	case sp_cst4:
		argval = get32();
		break;
	case sp_dnam:
	case sp_pnam:
		getstring(1);
		break;
	case sp_scon:
		getstring(0);
		break;
	case sp_doff:
		offtyp = getarg(sym_ptyp);
		getarg(cst_ptyp);
		break;
	case sp_icon:
	case sp_ucon:
	case sp_fcon:
		getarg(cst_ptyp);
		consiz = (long) argval;
		getstring(0);
		argval = consiz;
		break;
	}
	return(i);
}

int get16() {
	register int l_byte, h_byte;

	l_byte = get8();
	h_byte = get8();
	if ( h_byte>=128 ) h_byte -= 256 ;
	return l_byte | (h_byte*256) ;
}

long get32() {
	register long l;
	register int h_byte;

	l = get8(); l |= (unsigned)get8()*256 ; l |= get8()*256L*256L ;
	h_byte = get8() ;
	if ( h_byte>=128 ) h_byte -= 256 ;
	return l | (h_byte*256L*256*256L) ;
}

getstring(ident) {
	register char *p;
	register n;

	getarg(cst_ptyp);
	if (argval < 0 || argval > MAXSTR)
		fatal("string/identifier too long");
	strsiz = n = argval;
	p = string;
	while (--n >= 0)
		*p++ = get8();
	if (ident) {
		if (!isascii(string[0]) || !isalpha(string[0])) {
			identerror();
			return;
		}
		for (n=strsiz,p=string+1;--n>0;p++)
			if (!isascii(*p) || !isalnum(*p)) {
				identerror();
				return;
			}
	}
}

/* ----- output ----- */

putarg(t) {

	if (argnum != 0)
		putchar(argnum == 1 ? ' ' : ',');
	argnum++;
	puttyp(t);
}

puttyp(t) {

	switch (t) {
	case sp_ilb1:
	case sp_ilb2:
		printf("*%d",(int) argval);
		break;
	case sp_dlb1:
	case sp_dlb2:
		printf(".%d",dlbval);
		break;
	case sp_dnam:
		putstr(0,0);
		break;
	case sp_cst2:
	case sp_cst4:
		printf("%ld",argval);
		break;
	case sp_doff:
		puttyp(offtyp);
		if (argval >= 0) putchar('+');
		printf("%ld",argval);
		break;
	case sp_pnam:
		putstr('$',0);
		break;
	case sp_scon:
		putstr('\'','\'');
		break;
	case sp_icon:
		putstr(0,'I');
		printf("%ld",argval);
		break;
	case sp_ucon:
		putstr(0,'U');
		printf("%ld",argval);
		break;
	case sp_fcon:
		putstr(0,'F');
		printf("%ld",argval);
		break;
	case sp_cend:
		putchar('?');
		break;
	}
}

putstr(c,c2) register c; {
	register char *p;

	if (c)
		putchar(c);
	p = string;
	while (--strsiz >= 0) {
		c = *p++ & 0377;
		if (c >= 040 && c < 0177) {
			if (c == '\'' || c == '\\')
				putchar('\\');
			putchar(c);
		} else
			printf("\\%03o",c);
	}
	if (c2)
		putchar(c2);
}

/* ----- error handling ----- */

fail_check() {
	error("argument range error");
}

identerror() {
	error("'%s' is not a correct identifier",string);
}

/* VARARGS */
error(s,a1,a2,a3,a4) char *s; {
	fprintf(stderr,
		"%s: line %d: ",
		filename ? filename : progname,
		lineno);
	fprintf(stderr,s,a1,a2,a3,a4);
	fprintf(stderr,"\n");
	errors++;
}

/* VARARGS */
fatal(s,a1,a2,a3,a4) char *s; {
	error(s,a1,a2,a3,a4);
	exit(-1);
}
