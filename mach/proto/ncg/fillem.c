#ifndef NORCSID
static char rcsid2[] = "$Id$";
#endif

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <em_spec.h>
#include <em_pseu.h>
#include <em_flag.h>
#include <em_ptyp.h>
#include <em_mes.h>
#include "mach.h"
#include "param.h"
#include "tables.h"
#include "types.h"
#include <cgg_cg.h>
#include "data.h"
#include "glosym.h"
#include "result.h"
#ifdef REGVARS
#include "regvar.h"
#include <em_reg.h>
#endif
#ifdef USE_TES
#include "label.h"
#endif
#include "extern.h"

/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 *
 * Author: Hans van Staveren
 */

#ifndef newplb			/* retrofit for older mach.h */
#define newplb newilb
#endif

#ifdef fmt_id
#ifdef id_first
It is an error to define both fmt_id and id_first.
Read the documentation.
#endif
#endif

#ifdef fmt_ilb
#ifdef ilb_fmt
It is an error to define both fmt_ilb and ilb_fmt.
Read the documentation.
#endif
#endif

/* segment types for switchseg() */
#define SEGTXT          0
#define SEGCON          1
#define SEGROM          2
#define SEGBSS          3

#define get8()  getc(emfile)

static FILE *emfile;

static int nextispseu,savetab1;
static int opcode;
static int offtyp;
static long argval;
static int dlbval;
static char *str,argstr[128],labstr[128];
static unsigned int maxstrsiz;
static int strsiz;
static int holno=0;
static int procno=0;
static int curseg= -1;
static int part_size=0;
static word part_word=0;
#ifdef REGVARS
static int regallowed=0;
#endif

extern char em_flag[];
extern short em_ptyp[];

/*
 * Declare the machine dependent functions.
 *
 * These functions now return void, which is not compatible with
 * traditional K&R C.  Old mach.c files stop working until one fixes
 * them to return void, not int.
 */
void con_part(int, word);
void con_mult(word);
void con_float(void);
void prolog(full nlocals);
void mes(word);

static int getarg(int);
static int table1(void);
static int table2(void);
static int table3(int);
static int get16(void);
static long get32(void);
static void getstring(void);
static string strarg(int);
static void bss(full, int, int);
static long con(int);
static void switchseg(int);
static void savelab(void);
static void dumplab(void);
static void xdumplab(void);
static void part_flush(void);
static string holstr(word);

/* Own version of atol that continues computing on overflow.
   We don't know that about the ANSI C one.
*/
static long our_atol(char *s) {
  long total = 0;
  unsigned digit;
  int minus = 0;

  while (*s == ' ' || *s == '\t') s++;
  if (*s == '+') s++;
  else if (*s == '-') {
        s++;
        minus = 1;
  }
  while ((digit = *s++ - '0') < 10) {
        total *= 10;
        total += digit;
  }
  return(minus ? -total : total);
}


#define sp_cstx sp_cst2

void in_init(char *filename) {

	emfile = stdin;
	if (filename && (emfile=freopen(filename,"rb",stdin))==NULL)
		error("Can't open %s",filename);
	if (get16()!=sp_magic)
		error("Bad format %s",filename ? filename : "standard-input");
	str = myalloc(maxstrsiz=256);
}

void in_start(void) {
#ifdef modhead
	fprintf(codefile,"%s",modhead) ;
#endif
}

static void in_finish(void) {
}

void fillemlines(void) {
	int t,i;
	struct emline *lp;

	while ((emlines+nemlines)-emp<MAXEMLINES-5) {
		assert(nemlines<MAXEMLINES);
		if (nextispseu) {
			emlines[nemlines].em_instr=0;
			return;
		}
		lp = &emlines[nemlines++];

		switch(t=table1()) {
		default:
			error("unknown instruction byte");
		case sp_ilb1:
		case sp_ilb2:
#ifdef USE_TES
			lp->em_instr = op_lab;
			lp->em_optyp = OPSYMBOL;
			lp->em_soper = strarg(t);
			lp->em_u.em_loper = argval;
			return;
#endif
		case sp_fpseu:
		case sp_dlb1:
		case sp_dlb2:
		case sp_dnam:
			nextispseu=1; savetab1=t;
			nemlines--;
			lp->em_instr = 0;
			return;
		case EOF:
			nextispseu=1; savetab1=t;
			nemlines--;
			lp->em_instr = 0;
			return;
		case sp_fmnem:
			lp->em_instr = opcode;
			break;
		}
		i=em_flag[lp->em_instr-sp_fmnem] & EM_PAR;
		if ( i == PAR_NO ) {
			lp->em_optyp = OPNO;
			lp->em_soper = 0;
			continue;
		}
		t= em_ptyp[i];
		t= getarg(t);
		switch(i) {
		case PAR_L:
			assert(t == sp_cstx);
			if (argval >= 0)
				argval += TEM_BSIZE;
			lp->em_optyp = OPINT;
			lp->em_u.em_ioper = argval;
			lp->em_soper = tostring((word) argval);
			continue;
		case PAR_G:
			if (t != sp_cstx)
				break;
			lp->em_optyp = OPSYMBOL;
			lp->em_soper = holstr((word) argval);
			continue;
		case PAR_B:
			t = sp_ilb2;
#ifdef USE_TES
			lp->em_optyp = OPSYMBOL;
			lp->em_u.em_loper = argval;
			lp->em_soper = strarg(t);
#endif
			break;
		case PAR_D:
			assert(t == sp_cstx);
			lp->em_optyp = OPSYMBOL;
			lp->em_soper = strarg(t);
			lp->em_u.em_loper = argval;
			continue;
		}
		lp->em_soper = strarg(t);
		if (t==sp_cend)
			lp->em_optyp = OPNO;
		else if (t==sp_cstx) {
			lp->em_optyp = OPINT;
			lp->em_u.em_ioper = argval;
		} else
			lp->em_optyp = OPSYMBOL;
	}
}

void
dopseudo() {
	int b,t;
	full n;
	long save;
	word romcont[MAXROM+1];
	int nromwords;
	int rombit,rommask;

	if (nextispseu==0 || nemlines>0)
		error("No table entry for %d",emlines[0].em_instr);
	nextispseu=0;
	switch (savetab1) {
#ifndef USE_TES
	case sp_ilb1:
	case sp_ilb2:
		swtxt();
		/* dummy = */stackupto(&fakestack[stackheight-1],maxply,TRUE);
		cleanregs();
		strarg(savetab1);
		newilb(argstr);
#ifndef NDEBUG
		{ extern int Debug; extern char * strtdebug;
		if (strcmp(strtdebug,argstr)==0)
			Debug = strtdebug[-2]-'0';
		}
#endif
		return;
#endif
	case sp_dlb1:
	case sp_dlb2:
	case sp_dnam:
		strarg(savetab1);
		savelab();
		return;
	case sp_fpseu:
		break;
	case EOF:
		swtxt();
		in_finish();
		out_finish();
		popstr(0);
		tstoutput();
		exit(0);
	default:
		error("Unknown opcode %d",savetab1);
	}
	switch (opcode) {
	case ps_hol:
		sprintf(labstr,hol_fmt,++holno);
	case ps_bss:
		getarg(cst_ptyp);
		n = (full) argval;
		t = getarg(val_ptyp);
		save = argval;
		getarg(cst_ptyp);
		b = (int) argval;
		argval = save;
		bss(n,t,b);
		break;
	case ps_con:
		switchseg(SEGCON);
		dumplab();
		con(getarg(val_ptyp));
		while ((t = getarg(any_ptyp)) != sp_cend)
			con(t);
		break;
	case ps_rom:
		switchseg(SEGROM);
		xdumplab();
		nromwords=0;
		rommask=0;
		rombit=1;
		for (;;) {
			t=getarg(val_ptyp);
			while (t!=sp_cend) {
				if (t==sp_cstx && nromwords<MAXROM) {
					romcont[nromwords] = (word) argval;
					rommask |= rombit;
				}
				nromwords++;
				rombit <<= 1;
				con(t);
				t=getarg(any_ptyp);
			}
			{
				int c = get8();

				if (c == ps_rom) continue;
				if (c != EOF) ungetc(c, emfile);
			}
			break;
		}
		if (nromwords != 0) {
			romcont[MAXROM]=rommask;
			enterglo(labstr,romcont);
		}
		labstr[0]=0;
		break;
	case ps_mes:
		getarg(ptyp(sp_cst2));
		if (argval == ms_emx) {
			getarg(ptyp(sp_cst2));
			if (argval != TEM_WSIZE)
				fatal("bad word size");
			getarg(ptyp(sp_cst2));
			if (argval != TEM_PSIZE)
				fatal("bad pointer size");
			if ( getarg(any_ptyp)!=sp_cend )
				fatal("too many parameters");
#ifdef USE_TES
		} else if (argval == ms_tes) {
			int lbl, size, flthr;
			getarg(ptyp(sp_cst2)); lbl = argval;
			getarg(ptyp(sp_cst2)); size = argval;
			getarg(ptyp(sp_cst2)); flthr = argval;
			if ( getarg(any_ptyp)!=sp_cend )
				fatal("too many parameters");
			add_label(lbl,size, flthr);
#endif
#ifdef REGVARS
		} else if (argval == ms_gto) {
			getarg(ptyp(sp_cend));
			if (!regallowed)
				error("mes 3 not allowed here");
			fixregvars(TRUE);
			regallowed=0;
		} else if (argval == ms_reg) {
			long r_off;
			int r_size,r_type,r_score;

			if (!regallowed)
				error("mes 3 not allowed here");
			if(getarg(ptyp(sp_cst2)|ptyp(sp_cend)) == sp_cend) {
				fixregvars(FALSE);
				regallowed=0;
			} else {
				r_off = argval;
  				if (r_off >= 0)
  					r_off += TEM_BSIZE;
				getarg(ptyp(sp_cst2));
				r_size = argval;
				getarg(ptyp(sp_cst2));
				r_type = argval;
				if (r_type<reg_any || r_type>reg_float)
					fatal("Bad type in register message");
				if(getarg(ptyp(sp_cst2)|ptyp(sp_cend)) == sp_cend)
					r_score = 0;
				else {
					r_score = argval;
					if ( getarg(any_ptyp)!=sp_cend )
						fatal("too many parameters");
				}
				tryreg(linkreg(r_off,r_size,r_type,r_score),r_type);
			}
#endif
		} else
			mes((word)argval);
		break;
	case ps_exa:
		strarg(getarg(sym_ptyp));
		ex_ap(argstr);
		break;
	case ps_ina:
		strarg(getarg(sym_ptyp));
		in_ap(argstr);
		break;
	case ps_exp:
		strarg(getarg(ptyp(sp_pnam)));
		ex_ap(argstr);
		break;
	case ps_inp:
		strarg(getarg(ptyp(sp_pnam)));
		in_ap(argstr);
		break;
	case ps_pro:
		switchseg(SEGTXT);
		procno++;
		strarg(getarg(ptyp(sp_pnam)));
		newplb(argstr);
		getarg(cst_ptyp);
		prolog((full)argval);
#ifdef REGVARS
		regallowed++;
#endif
		break;
	case ps_end:
		getarg(cst_ptyp | ptyp(sp_cend));
#ifdef USE_TES
		kill_labels();
#endif
		cleanregs();
#ifdef REGVARS
		unlinkregs();
#endif
		tstoutput();
		break;
	default:
		error("No table entry for %d",savetab1);
	}
}

/* ----- input ----- */

static int getarg(int typset) {
	int t,argtyp;

	argtyp = t = table2();
	if (t == EOF)
		fatal("unexpected EOF");
	t -= sp_fspec;
	t = 1 << t;
	if ((typset & t) == 0)
		error("bad argument type %d",argtyp);
	return(argtyp);
}

static int table1(void) {
	int i;

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

static int table2(void) {
	int i;

	i = get8();
	if (i < sp_fcst0+sp_ncst0 && i >= sp_fcst0) {
		argval = i - sp_zcst0;
		return(sp_cstx);
	}
	return(table3(i));
}

static int table3(int i) {
	word consiz;

	switch(i) {
	case sp_ilb1:
		argval = get8();
		break;
	case sp_dlb1:
		dlbval = get8();
		break;
	case sp_dlb2:
		dlbval = get16();
		break;
	case sp_cst2:
		i = sp_cstx;
	case sp_ilb2:
		argval = get16();
		break;
	case sp_cst4:
		i = sp_cstx;
		argval = get32();
		break;
	case sp_dnam:
	case sp_pnam:
	case sp_scon:
		getstring();
		break;
	case sp_doff:
		offtyp = getarg(sym_ptyp);
		getarg(cst_ptyp);
		break;
	case sp_icon:
	case sp_ucon:
	case sp_fcon:
		getarg(cst_ptyp);
		consiz = (word) argval;
		getstring();
		argval = consiz;
		break;
	}
	return(i);
}

static int get16(void) {
	int l_byte, h_byte;

	l_byte = get8();
	h_byte = get8();
	if ( h_byte>=128 ) h_byte -= 256 ;
	return l_byte | (h_byte*256) ;
}

static long get32(void) {
	long l;
	int h_byte;

	l = get8();
	l |= ((unsigned) get8())*256 ;
	l |= get8()*256L*256L ;
	h_byte = get8() ;
	if ( h_byte>=128 ) h_byte -= 256 ;
	return l | (h_byte*256L*256*256L) ;
}

static void getstring(void) {
	char *p;
	int n;

	getarg(cst_ptyp);
	if (argval < 0)
		fatal("string/identifier too long");
	if (argval >= maxstrsiz) {
		myfree(str);
		str = myalloc((unsigned) argval + 1);
		maxstrsiz = argval + 1;
	}
	strsiz = n = (int) argval;
	p = str;
	while (--n >= 0)
		*p++ = get8();
	*p++ = '\0';
}

static string strarg(int t) {
	char *p;

	switch (t) {
	case sp_ilb1:
	case sp_ilb2:
#ifdef fmt_ilb
		fmt_ilb(procno,((int) argval),argstr);
#else
		sprintf(argstr,ilb_fmt,procno,(int)argval);
#endif
		break;
	case sp_dlb1:
	case sp_dlb2:
		sprintf(argstr,dlb_fmt,dlbval);
		break;
	case sp_cstx:
		sprintf(argstr,cst_fmt,(full)argval);
		break;
	case sp_dnam:
	case sp_pnam:
#ifdef fmt_id
		fmt_id(str,argstr);
#else
		p = argstr;
		if (strsiz < 8 || str[0] == id_first)
			*p++ = id_first;
		sprintf(p,"%.*s",strsiz,str);
#endif
		break;
	case sp_doff:
		strarg(offtyp);
		for (p = argstr; *p; p++)
			;
		if ((full) argval >= 0)
			*p++ = '+';
		else {
			*p++ = '-';
			argval = - (full) argval;
		}
		sprintf(p,off_fmt,(full)argval);
		break;
	case sp_cend:
		return("");
	}
	return(mystrcpy(argstr));
}

static void bss(full n, int t, int b) {
	long s = 0;

	if (n % TEM_WSIZE)
		fatal("bad BSS size");
	if (b==0
#ifdef BSS_INIT
	    || (t==sp_cstx && argval==BSS_INIT)
#endif /* BSS_INIT */
		) {
		switchseg(SEGBSS);
		newlbss(labstr,n);
		labstr[0]=0;
		return;
	}
	switchseg(SEGCON);
	dumplab();
	while (n > 0)
		n -= (s = con(t));
	if (s % TEM_WSIZE)
		fatal("bad BSS initializer");
}

static long con(int t) {
	int i;

	strarg(t);
	switch (t) {
	case sp_ilb1:
	case sp_ilb2:
	case sp_pnam:
		part_flush();
		con_ilb(argstr);
		return((long)TEM_PSIZE);
	case sp_dlb1:
	case sp_dlb2:
	case sp_dnam:
	case sp_doff:
		part_flush();
		con_dlb(argstr);
		return((long)TEM_PSIZE);
	case sp_cstx:
		con_part(TEM_WSIZE,(word)argval);
		return((long)TEM_WSIZE);
	case sp_scon:
		for (i = 0; i < strsiz; i++)
			con_part(1,(word) str[i]);
		return((long)strsiz);
	case sp_icon:
	case sp_ucon:
		if (argval > TEM_WSIZE) {
			part_flush();
			con_mult((word)argval);
		} else {
			con_part((int)argval,(word)our_atol(str));
		}
		return(argval);
	case sp_fcon:
		part_flush();
		con_float();
		return(argval);
	}
	assert(FALSE);
	/* NOTREACHED */
}

extern char *segname[];

void swtxt(void) {
	switchseg(SEGTXT);
}

static void switchseg(int s) {

	if (s == curseg)
		return;
	part_flush();
	if ((curseg = s) >= 0)
		fprintf(codefile,"%s\n",segname[s]);
}

static void savelab(void) {
	register char *p,*q;

	part_flush();
	if (labstr[0]) {
		dlbdlb(argstr,labstr);
		return;
	}
	p = argstr;
	q = labstr;
	while ((*q++ = *p++) != '\0')
		continue;
}

static void dumplab(void) {

	if (labstr[0] == 0)
		return;
	assert(part_size == 0);
	newdlb(labstr);
	labstr[0] = 0;
}

static void xdumplab(void) {

	if (labstr[0] == 0)
		return;
	assert(part_size == 0);
	newdlb(labstr);
}

static void part_flush(void) {

	/*
	 * Each new data fragment and each data label starts at
	 * a new target machine word
	 */
	if (part_size == 0)
		return;
	con_cst(part_word);
	part_size = 0;
	part_word = 0;
}

static string holstr(word n) {

	sprintf(str,hol_off,n,holno);
	return(mystrcpy(str));
}


/* ----- machine dependent routines ----- */

#include        "mach.c"
