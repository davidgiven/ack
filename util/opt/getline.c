#ifndef NORCSID
static char rcsid[] = "$Id$";
#endif

#include <stdlib.h>
#include <stdio.h>
#include "param.h"
#include "types.h"
#include "tes.h"
#include "line.h"
#include "lookup.h"
#include "alloc.h"
#include "proinf.h"
#include <em_spec.h>
#include <em_pseu.h>
#include <em_flag.h>
#include <em_mes.h>
#include "ext.h"

/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 *
 * Author: Hans van Staveren
 */


static  short   tabval;         /* temp store for shorts */
static  offset  tabval2;        /* temp store for offsets */
static  char    string[IDL+1];  /* temp store for names */

/*
 * The next constants are close to sp_cend for fast switches
 */
#define INST    256     /* instruction:         number in tabval */
#define PSEU    257     /* pseudo:              number in tabval */
#define ILBX    258     /* label:               number in tabval */
#define DLBX    259     /* symbol:              name in string[] */
#define CSTX1   260     /* short constant:      stored in tabval */
#define CSTX2   261     /* offset:              value in tabval2 */
#define VALX1   262     /* symbol+short:        in string[] and tabval */
#define VALX2   263     /* symbol+offset:       in string[] and tabval2 */
#define ATEOF   264     /* bumped into end of file */

#define readbyte getchar

short readshort() {
	register int l_byte, h_byte;

	l_byte = readbyte();
	h_byte = readbyte();
	if ( h_byte>=128 ) h_byte -= 256 ;
	return l_byte | (h_byte*256) ;
}

#ifdef LONGOFF
offset readoffset() {
	register long l;
	register int h_byte;

	l = readbyte();
	l |= ((unsigned) readbyte())*256 ;
	l |= readbyte()*256L*256L ;
	h_byte = readbyte() ;
	if ( h_byte>=128 ) h_byte -= 256 ;
	return l | (h_byte*256L*256*256L) ;
}
#endif

draininput() {

	/*
	 * called when MES ERR is encountered.
	 * Drain input in case it is a pipe.
	 */

	while (getchar() != EOF)
		;
}

short getint() {

	switch(table2()) {
	default: error("int expected");
	case CSTX1:
		return(tabval);
	}
}

sym_p getsym(status) int status; {

	switch(table2()) {
	default:
		error("symbol expected");
	case DLBX:
		return(symlookup(string,status,0));
	case sp_pnam:
		return(symlookup(string,status,SYMPRO));
	}
}

offset getoff() {

	switch (table2()) {
	default: error("offset expected");
	case CSTX1:
		return((offset) tabval);
#ifdef LONGOFF
	case CSTX2:
		return(tabval2);
#endif
	}
}

make_string(n) int n; {

	sprintf(string,".%u",n);
}

inident() {
	register n;
	register char *p = string;
	register c;

	n = getint();
	while (n--) {
		c = readbyte();
		if (p<&string[IDL])
			*p++ = c;
	}
	*p++ = 0;
}

int table3(n) int n; {

	switch (n) {
	case sp_ilb1:   tabval = readbyte(); return(ILBX);
	case sp_ilb2:   tabval = readshort(); return(ILBX);
	case sp_dlb1:   make_string(readbyte()); return(DLBX);
	case sp_dlb2:   make_string(readshort()); return(DLBX);
	case sp_dnam:   inident(); return(DLBX);
	case sp_pnam:   inident(); return(n);
	case sp_cst2:   tabval = readshort(); return(CSTX1);
#ifdef LONGOFF
	case sp_cst4:   tabval2 = readoffset(); return(CSTX2);
#endif
	case sp_doff:   if (table2()!=DLBX) error("symbol expected");
			switch(table2()) {
			default:        error("offset expected");
			case CSTX1:             return(VALX1);
#ifdef LONGOFF
			case CSTX2:             return(VALX2);
#endif
			}
	default:        return(n);
	}
}

int table1() {
	register n;

	n = readbyte();
	if (n == EOF)
		return(ATEOF);
	if ((n <= sp_lmnem) && (n >= sp_fmnem)) {
		tabval = n;
		return(INST);
	}
	if ((n <= sp_lpseu) && (n >= sp_fpseu)) {
		tabval = n;
		return(PSEU);
	}
	if ((n < sp_filb0 + sp_nilb0) && (n >= sp_filb0)) {
		tabval = n - sp_filb0;
		return(ILBX);
	}
	return(table3(n));
}

int table2() {
	register n;

	n = readbyte();
	if ((n < sp_fcst0 + sp_ncst0) && (n >= sp_fcst0)) {
		tabval = n - sp_zcst0;
		return(CSTX1);
	}
	return(table3(n));
}

void
getlines() {
	register line_p lnp;
	register instr;

    for(;;) {
	linecount++;
	switch(table1()) {
	default:
		error("unknown instruction byte");
		/* NOTREACHED */

	case ATEOF:
		if (prodepth!=0)
			error("procedure unterminated at eof");
		process();
		return;
	case INST:
		tstinpro();
		instr = tabval;
		break;
	case DLBX:
		lnp = newline(OPSYMBOL);
		lnp->l_instr = ps_sym;
		lnp->l_a.la_sp= symlookup(string,DEFINING,0);
		lnp->l_next = curpro.lastline;
		curpro.lastline = lnp;
		continue;
	case ILBX:
		tstinpro();
		lnp = newline(OPNUMLAB);
		lnp->l_instr = op_lab;
		lnp->l_a.la_np = numlookup((unsigned) tabval);
		if (lnp->l_a.la_np->n_line != (line_p) 0)
			error("label %u multiple defined",(unsigned) tabval);
		lnp->l_a.la_np->n_line = lnp;
		lnp->l_next = curpro.lastline;
		curpro.lastline = lnp;
		continue;
	case PSEU:
		if(inpseudo(tabval))
			return;
		continue;
	}

	/*
	 * Now we have an instruction number in instr
	 * There might be an operand, look for it
	 */

	if ((em_flag[instr-sp_fmnem]&EM_PAR)==PAR_NO) {
		lnp = newline(OPNO);
	} else switch(table2()) {
	default:
		error("unknown offset byte");
	case sp_cend:
		lnp = newline(OPNO);
		break;
	case CSTX1:
		if ((em_flag[instr-sp_fmnem]&EM_PAR)!= PAR_B) {
			if (CANMINI(tabval))
				lnp = newline(tabval+Z_OPMINI);
			else {
				lnp = newline(OPSHORT);
				lnp->l_a.la_short = tabval;
			}
		} else {
			lnp = newline(OPNUMLAB);
			lnp->l_a.la_np = numlookup((unsigned) tabval);
		}
		break;
#ifdef LONGOFF
	case CSTX2:
		lnp = newline(OPOFFSET);
		lnp->l_a.la_offset = tabval2;
		break;
#endif
	case ILBX:
		tstinpro();
		lnp = newline(OPNUMLAB);
		lnp->l_a.la_np = numlookup((unsigned) tabval);
		break;
	case DLBX:
		lnp = newline(OPSYMBOL);
		lnp->l_a.la_sp = symlookup(string,OCCURRING,0);
		break;
	case sp_pnam:
		lnp = newline(OPSYMBOL);
		lnp->l_a.la_sp = symlookup(string,OCCURRING,SYMPRO);
		break;
	case VALX1:
		lnp = newline(OPSVAL);
		lnp->l_a.la_sval.lasv_sp = symlookup(string,OCCURRING,0);
		lnp->l_a.la_sval.lasv_short = tabval;
		break;
#ifdef LONGOFF
	case VALX2:
		lnp = newline(OPLVAL);
		lnp->l_a.la_lval.lalv_sp = symlookup(string,OCCURRING,0);
		lnp->l_a.la_lval.lalv_offset = tabval2;
		break;
#endif
	}
	lnp->l_instr = instr;
	lnp->l_next = curpro.lastline;
	curpro.lastline = lnp;
    }
}

argstring(length,abp) offset length; register argb_p abp; {

	while (length--) {
		if (abp->ab_index == NARGBYTES)
			abp = abp->ab_next = newargb();
		abp->ab_contents[abp->ab_index++] = readbyte();
	}
}

line_p  arglist(n) int n; {
	line_p  lnp;
	register arg_p ap,*app;
	bool moretocome;
	offset length;


	/*
	 * creates an arglist with n elements
	 * if n == 0 the arglist is variable and terminated by sp_cend
	 */

	lnp = newline(OPLIST);
	app = &lnp->l_a.la_arg;
	moretocome = TRUE;
	do {
		switch(table2()) {
		default:
			error("unknown byte in arglist");
		case CSTX1:
			tabval2 = (offset) tabval;
		case CSTX2:
			*app = ap = newarg(ARGOFF);
			ap->a_a.a_offset = tabval2;
			app = &ap->a_next;
			break;
		case ILBX:
			tstinpro();
			*app = ap = newarg(ARGNUM);
			ap->a_a.a_np = numlookup((unsigned) tabval);
			ap->a_a.a_np->n_flags |= NUMDATA;
			app = &ap->a_next;
			break;
		case DLBX:
			*app = ap = newarg(ARGSYM);
			ap->a_a.a_sp = symlookup(string,OCCURRING,0);
			app = &ap->a_next;
			break;
		case sp_pnam:
			*app = ap = newarg(ARGSYM);
			ap->a_a.a_sp = symlookup(string,OCCURRING,SYMPRO);
			app = &ap->a_next;
			break;
		case VALX1:
			tabval2 = (offset) tabval;
		case VALX2:
			*app = ap = newarg(ARGVAL);
			ap->a_a.a_val.av_sp = symlookup(string,OCCURRING,0);
			ap->a_a.a_val.av_offset = tabval2;
			app = &ap->a_next;
			break;
		case sp_scon:
			*app = ap = newarg(ARGSTR);
			length = getoff();
			argstring(length,&ap->a_a.a_string);
			app = &ap->a_next;
			break;
		case sp_icon:
			*app = ap = newarg(ARGICN);
			goto casecon;
		case sp_ucon:
			*app = ap = newarg(ARGUCN);
			goto casecon;
		case sp_fcon:
			*app = ap = newarg(ARGFCN);
		casecon:
			length = getint();
			ap->a_a.a_con.ac_length = (short) length;
			argstring(getoff(),&ap->a_a.a_con.ac_con);
			app = &ap->a_next;
			break;
		case sp_cend:
			moretocome = FALSE;
		}
		if (n && (--n) == 0)
			moretocome = FALSE;
	} while (moretocome);
	return(lnp);
}

offset aoff(ap,n) register arg_p ap; {

	while (n>0) {
		if (ap != (arg_p) 0)
			ap = ap->a_next;
		n--;
	}
	if (ap == (arg_p) 0)
		error("too few parameters");
	if (ap->a_typ != ARGOFF)
		error("offset expected");
	return(ap->a_a.a_offset);
}

int inpseudo(n) short n; {
	register line_p lnp,head,tail;
	short           n1,n2;
	proinf savearea;
#ifdef PSEUBETWEEN
	static int pcount=0;

	if (pcount++ >= PSEUBETWEEN && prodepth==0) {
		process();
		pcount=0;
	}
#endif

	switch(n) {
	default:
		error("unknown pseudo");
	case ps_bss:
	case ps_hol:
		lnp = arglist(3);
		break;
	case ps_rom:
	case ps_con:
		lnp = arglist(0);
		break;
	case ps_ina:
	case ps_inp:
	case ps_exa:
	case ps_exp:
		lnp = newline(OPSYMBOL);
		lnp->l_a.la_sp = getsym(NOTHING);
		break;
	case ps_exc:
		n1 = getint(); n2 = getint();
		if (n1 != 0 && n2 != 0) {
			tail = curpro.lastline;
			while (--n2) tail = tail->l_next;
			head = tail;
			while (n1--) head = head->l_next;
			lnp = tail->l_next;
			tail->l_next = head->l_next;
			head->l_next = curpro.lastline;
			curpro.lastline = lnp;
		}
		lnp = newline(OPNO);
		break;
	case ps_mes:
		lnp = arglist(0);
		switch((int) aoff(lnp->l_a.la_arg,0)) {
		case ms_err:
			draininput(); exit(-1);
		case ms_opt:
			nflag = TRUE; break;
		case ms_emx:
			wordsize = aoff(lnp->l_a.la_arg,1);
			pointersize = aoff(lnp->l_a.la_arg,2);
#ifndef LONGOFF
			if (wordsize>2)
				error("This optimizer cannot handle wordsize>2");
#endif
			break;
		case ms_gto:
			curpro.gtoproc=1;
			/* Treat as empty mes ms_reg */
		case ms_reg:
			tstinpro();
			regvar(lnp->l_a.la_arg->a_next);
			oldline(lnp);
			lnp=newline(OPNO);
			n=ps_exc;	/* kludge to force out this line */
			break;
		case ms_tes:
			tstinpro();
			oldline(lnp);
			lnp=newline(OPNO);
			n=ps_exc;	/* kludge to force out this line */
			break;
		}
		break;
	case ps_pro:
		if (prodepth>0)
			savearea = curpro;
		else
			process();
		curpro.symbol = getsym(DEFINING);
		switch(table2()) {
		case sp_cend:
			curpro.localbytes = (offset) -1;
			break;
		case CSTX1:
			tabval2 = (offset) tabval;
		case CSTX2:
			curpro.localbytes = tabval2;
			break;
		default:
			error("bad second arg of PRO");
		}
		prodepth++;
		curpro.gtoproc=0;
		if (prodepth>1) {
			register i;

			curpro.lastline = (line_p) 0;
			curpro.freg = (reg_p) 0;
			for(i=0;i<NNUMHASH;i++)
				curpro.numhash[i] = (num_p) 0;
			getlines();
			curpro = savearea;
			prodepth--;
		}
		return(0);
	case ps_end:
		if (prodepth==0)
			error("END misplaced");
		switch(table2()) {
		case sp_cend:
			if (curpro.localbytes == (offset) -1)
				error("bytes for locals still unknown");
			break;
		case CSTX1:
			tabval2 = (offset) tabval;
		case CSTX2:
			if (curpro.localbytes != (offset) -1 && curpro.localbytes != tabval2)
				error("inconsistency in number of bytes for locals");
			curpro.localbytes = tabval2;
			break;
		}
		process();
		curpro.symbol = (sym_p) 0;
		if (prodepth==1) {
			prodepth=0;
#ifdef PSEUBETWEEN
			pcount=0;
#endif
			return(0);
		} else
			return(1);
	}
	lnp->l_instr = n;
	lnp->l_next = curpro.lastline;
	curpro.lastline = lnp;
	return(0);
}

tstinpro() {

	if (prodepth==0)
		error("This is not allowed outside a procedure");
}
