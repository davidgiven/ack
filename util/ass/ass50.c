/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 *
 */

#include        "ass00.h"
#include        "assex.h"
#include		"assrl.h"
#include        "ip_spec.h"

/*
** Pass 5 of EM1 assembler/loader
** Fix reloc tables
** Write out code
*/

static void patchcase(void);



void pass_5(void)
{
	register line_t *lnp;
	cons_t off1;
	char defined ;
	int afterlength, partype ;
	register int inslength, ope;
	char *op_curr ;

	pass = 5;
	afterlength = 0;
	for (lnp = pstate.s_fline ; lnp ; lnp= lnp->l_next, line_num++ ) {
		ope = ctrunc(lnp->instr_num);
		if ( ope==sp_ilb1 ) continue ;
		if ( ope==sp_fpseu ) {
			line_num = lnp->ad.ad_ln.ln_first ;
			continue ;
		}
		off1 = parval(lnp,&defined);
		if ( (op_curr = lnp->opoff)==NO_OFF ) {
			fatal("opoff assertion failed") ;
		}
		inslength = oplength(*op_curr) ;
		afterlength += inslength ;

		/*
		 * Change absolute offset to a relative for branches.
		 */


		partype= em_flag[ope]&EM_PAR ;
		if ( partype==PAR_B && defined ) {
			off1 -= afterlength;
		}

#ifdef JOHAN
		if ( jflag ) {
			extern char em_mnem[][4] ;
			printf("%s %ld\n",em_mnem[ope],off1) ;
		}
#endif

		if ( !defined && partype==PAR_G ) { /* must be external */
			text_reloc((lnp->type1==GLOSYM ?
					lnp->ad.ad_gp:lnp->ad.ad_df.df_gp),
				(FOFFSET)(textbytes+afterlength-inslength) ,
					op_curr-opchoice);
			xputarb(inslength,off1,tfile);
			textoff += inslength ;
		} else {
			genop(op_curr,off1,partype) ;
		}
	} /* end forloop */
	line_num-- ;

	patchcase();
	textbytes += prog_size;
	if ( textbytes>maxadr ) fatal("Maximum code area size exceeded") ;

} /* end pass_5 */


/**
 * Real code generation.
 */
void genop(char *startc,cons_t value,int i_flag)
{
	char *currc ;
	register int flag ;
	char opc ;

	currc= startc ;
	flag = ctrunc(*currc++);
	opc  = *currc++;
	if ( (flag&OPTYPE)!=OPNO ) {

		if ( !opfit(flag,*currc,value,i_flag) ) {
		   fatal("parameter value unsuitable for selected opcode") ;
		}
		if ( flag&OPWORD ) {
			if ( value%wordsize!=0 ) {
				error("parameter not word multiple");
			}
			value /= wordsize ;
		}
		if ( flag&OPNZ ) {
			if ( value<=0 ) error("negative parameter");
			value-- ;
		}
	}
	if ( flag&OPESC ) put8(ESC) ;

	switch ( flag&OPTYPE ) {
	case OPMINI :
		opc += value<0 ? -1-value : value ;
		break ;
	case OPSHORT :
		if ( value<0 ) {
			opc += -1-(value>>8) ;
		} else {
			opc += value>>8 ;
		}
		break ;
	case OP32 :
	case OP64 :
		put8(ESC_L) ;
	}

#ifdef DUMP
	if ( c_flag ) {
		switch(flag&OPTYPE) {
		case OP32 :
		case OP64 :
			opcnt3[opc&0377]= 1 ;
			break ;
		default :
			if ( flag&OPESC ) opcnt2[opc&0377]= 1 ;
			else              opcnt1[opc&0377]= 1 ;
			break ;
		}
	}
#endif

	put8(opc) ;
	switch( flag&OPTYPE ) {
	case OPNO:
	case OPMINI:
		break ;
	case OPSHORT:
	case OP8:
		put8((char)value) ;
		break ;
	case OP16:
	case OP16U:
		put16(int_cast value) ;
		break ;
	case OP32:
		put32(value) ;
		break ;
	case OP64:
		put64(value) ;
		break ;
	}
}

static void patchcase(void)
{
	register relc_t *r;
	register locl_t *k;

	if ( (r= pstate.s_fdata) ) {
		r= r->r_next ;
	} else {
		r= f_data ;
	}
	for( ; r ; r= r->r_next ) {
		if (r->r_typ == RELLOC) {
			r->r_typ = RELADR;
			k = r->r_val.rel_lp;
			if (k->l_defined==YES)
				r->r_val.rel_i = k->l_min + textbytes;
			else
				error("case label at line %d undefined",
					k->l_min);
		}
	}
}
