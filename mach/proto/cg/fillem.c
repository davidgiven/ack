#ifndef NORCSID
static char rcsid2[] = "$Header$";
#endif

#include <stdio.h>
#include "assert.h"
#include <em_spec.h>
#include <em_pseu.h>
#include <em_flag.h>
#include <em_ptyp.h>
#include <em_mes.h>
#include "mach.h"
#include "param.h"
#include "tables.h"
#include "types.h"
#include <cg_pattern.h>
#include "data.h"
#include "result.h"
#ifdef REGVARS
#include "regvar.h"
#include <em_reg.h>
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

long con();

#define get8()  getc(emfile)

#define MAXSTR 256

FILE *emfile;
extern FILE *codefile;
extern FILE *freopen();

int nextispseu,savetab1;
int opcode;
int offtyp;
long argval;
int dlbval;
char str[MAXSTR],argstr[128],labstr[128];
int strsiz;
int holno=0;
int procno=0;
int curseg= -1;
int part_size=0;
word part_word=0;
int endofprog=0;
#ifdef REGVARS
int regallowed=0;
#endif

extern char em_flag[];
extern short em_ptyp[];
extern long atol();
extern double atof();

#define sp_cstx sp_cst2

string tostring();
string holstr();
string strarg();
string mystrcpy();
long get32();

in_init(filename) char *filename; {

	if ((emfile=freopen(filename,"r",stdin))==NULL)
		error("Can't open %s",filename);
	if (get16()!=sp_magic)
		error("Bad format %s",filename);
}

in_start() {
#ifdef modhead
	fprintf(codefile,"%s",modhead) ;
#endif
}

in_finish() {
}

fillemlines() {
	int t,i;
	register struct emline *lp;

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
			endofprog=1;
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

dopseudo() {
	register b,t;
	register full n;
	register long save;
	word romcont[MAXROM+1];
	int nromwords;
	int rombit,rommask;
	unsigned dummy,stackupto();

	if (nextispseu==0 || nemlines>0)
		error("No table entry for %d",emlines[0].em_instr);
	nextispseu=0;
	switch(savetab1) {
	case sp_ilb1:
	case sp_ilb2:
		swtxt();
		dummy = stackupto(&fakestack[stackheight-1],maxply,TRUE);
		cleanregs();
		strarg(savetab1);
		newilb(argstr);
		return;
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
				if (c !=EOF) ungetc(c, emfile);
			}
			break;
		}
		if (rommask != 0) {
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
			struct regvar *linkreg();

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

int getarg(typset) {
	register t,argtyp;

	argtyp = t = table2();
	if (t == EOF)
		fatal("unexpected EOF");
	t -= sp_fspec;
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
		return(sp_cstx);
	}
	return(table3(i));
}

int table3(i) {
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

	l = get8();
	l |= ((unsigned) get8())*256 ;
	l |= get8()*256L*256L ;
	h_byte = get8() ;
	if ( h_byte>=128 ) h_byte -= 256 ;
	return l | (h_byte*256L*256*256L) ;
}

getstring() {
	register char *p;
	register n;

	getarg(cst_ptyp);
	if (argval < 0 || argval > MAXSTR-1)
		fatal("string/identifier too long");
	strsiz = n = (int) argval;
	p = str;
	while (--n >= 0)
		*p++ = get8();
	*p++ = '\0';
}

char *strarg(t) {
	register char *p;

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

bss(n,t,b) full n; {
	register long s;

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

long con(t) {
	register i;

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
			con_part((int)argval,(word)atol(str));
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

swtxt() {
	switchseg(SEGTXT);
}

switchseg(s) {

	if (s == curseg)
		return;
	part_flush();
	if ((curseg = s) >= 0)
		fprintf(codefile,"%s\n",segname[s]);
}

savelab() {
	register char *p,*q;

	part_flush();
	if (labstr[0]) {
		dlbdlb(argstr,labstr);
		return;
	}
	p = argstr;
	q = labstr;
	while (*q++ = *p++)
		;
}

dumplab() {

	if (labstr[0] == 0)
		return;
	assert(part_size == 0);
	newdlb(labstr);
	labstr[0] = 0;
}

xdumplab() {

	if (labstr[0] == 0)
		return;
	assert(part_size == 0);
	newdlb(labstr);
}

part_flush() {

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

string holstr(n) word n; {

	sprintf(str,hol_off,n,holno);
	return(mystrcpy(str));
}


/* ----- machine dependent routines ----- */

#include        "mach.c"
