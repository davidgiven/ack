#ifndef NORCSID
static char rcsid[] = "$Header$";
#endif

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

/*
 * machine dependent back end routines for the Motorola 68000
 */

#define IEEEFLOAT

#ifdef IEEEFLOAT
#include "FP.h"
#include "trp.c"
#include "fcompact.c"
#include "dbl_extract.c"
#ifdef	PRT_EXP_DEBUG
#include "prt_extend.c"
#endif
#endif IEEEFLOAT


con_part(sz,w) register sz; word w; {

	while (part_size % sz)
		part_size++;
	if (part_size == TEM_WSIZE)
		part_flush();
	if (sz == 1) {
		w &= 0xFF;
		if (part_size == 0)
			w <<= 8;
		part_word |= w;
	} else {
		assert(sz == 2);
		part_word = w;
	}
	part_size += sz;
}

con_mult(sz) word sz; {

	if (sz != 4)
		fatal("bad icon/ucon size");
	fprintf(codefile,".long %s\n",str);
}

#ifdef IEEEFLOAT
dbl_adjust(fl)
my_dbl	*fl;
{
	EXTEND	buf;

		/* special routine to strip SGL_BIAS */
	dbl_extract(fl,&buf);
		/* standard routine to add DBL_BIAS */
	fcompact(&buf,fl,sizeof(double));
}
#endif IEEEFLOAT

con_float()
{
	register word	sz;
	register long	*l;
#ifdef IEEEFLOAT
	register my_dbl	*md;
#endif IEEEFLOAT
		 double	d;
	char	mesg[128];

	sz = argval;
	if (sz!= 4 && sz!= 8) {
		sprintf(mesg,"con_float(): bad fcon size %d %D\nstr: %s\n\0",
				sz,sz,str);
		fatal(mesg);
	}

	d = atof(str);
	l = (long *) &d;

#ifdef IEEEFLOAT
	if (sz == 8)	{
		/* ATOF() RETURNS THE PROPER FORMAT FOR A FLOAT */
		/* BUT NOT FOR A DOUBLE. CORRECT THE FORMAT.	*/
		md = (my_dbl *) &d;
		dbl_adjust(md);
	}
#endif IEEEFLOAT

	while ( sz ) {
		fprintf(codefile,"\t.word 0x%x,0x%x !float test %s\n",
			(int)(*l)&0xFFFF,(int)(*l>>16)&0xFFFF,str);
		sz -=4 ;
		l++;
	}
}

#ifdef REGVARS

regscore(off,size,typ,score,totyp)
	long off;
{
	if (score == 0) return -1;
	switch(typ) {
		case reg_float:
			return -1;
		case reg_pointer:
			if (size != 4 || totyp != reg_pointer) return -1;
			score *= 2;
			break;
		case reg_loop:
			score += 5;
			/* fall through .. */
		case reg_any:
			if (size != 2 || totyp == reg_pointer) return -1;
			break;
	}
	if (off >= 0) {
		/* parameters must be initialised with an instruction
		 * like "move.w 4(a6),d0", which costs 2 words.
		 */
		score -= 2;
	}
	score -= 1; /* take save/restore into account */
	return score;
}
struct regsav_t {
	char	*rs_reg;	/* e.g. "a3" or "d5" */
	long	rs_off;		/* offset of variable */
	int	rs_size;	/* 2 or 4 bytes */
} regsav[9];


int regnr;

i_regsave()
{
	regnr = 0;
}

#define MOVEM_LIMIT	2
/* If #registers to be saved exceeds MOVEM_LIMIT, we
* use the movem instruction to save registers; else
* we simply use several move.l's.
*/

save()
{
	register struct regsav_t *p;

	if (regnr > MOVEM_LIMIT) {
		fprintf(codefile,"movem.l ");
		for (p = regsav; ;) {
			fprintf(codefile,"%s",p->rs_reg);
			if (++p == &regsav[regnr]) break;
			putc('/',codefile);
		}
		fprintf(codefile,",-(sp)\n");
	} else {
		for (p = regsav; p < &regsav[regnr]; p++) {
			fprintf(codefile,"move.l %s,-(sp)\n",p->rs_reg);
		}
	}
	/* initialise register-parameters */
	for (p = regsav; p < &regsav[regnr]; p++) {
		if (p->rs_off >= 0) {
			fprintf(codefile,"move.%c %ld(a6),%s\n",
				(p->rs_size == 4 ? 'l' : 'w'),
				p->rs_off,
				p->rs_reg);
		}
	}
}

restr()
{
	register struct regsav_t *p;

	if (regnr > MOVEM_LIMIT)  {
		fprintf(codefile,"movem.l (sp)+,");
		for (p = regsav; ;) {
			fprintf(codefile,"%s",p->rs_reg);
			if (++p == &regsav[regnr]) break;
			putc('/',codefile);
		}
		putc('\n',codefile);
	} else {
		for (p = &regsav[regnr-1]; p >= regsav; p--) {
			fprintf(codefile,"move.l (sp)+,%s\n",p->rs_reg);
		}
	}
	fprintf(codefile,"unlk a6\n");
	fprintf(codefile,"rts\n");
}


f_regsave()
{
	save();
}

regsave(str,off,size)
	char *str;
	long off;
{
	assert (regnr < 9);
	regsav[regnr].rs_reg = str;
	regsav[regnr].rs_off = off;
	regsav[regnr++].rs_size = size;
	fprintf(codefile, "!Local %ld into %s\n",off,str);
}

regreturn()
{
	restr();
}

#endif

prolog(nlocals) full nlocals; {

	fprintf(codefile,"tst.b -%D(sp)\nlink\ta6,#-%D\n",nlocals+40,nlocals);
}



mes(type) word type ; {
	int argt ;

	switch ( (int)type ) {
	case ms_ext :
		for (;;) {
			switch ( argt=getarg(
			    ptyp(sp_cend)|ptyp(sp_pnam)|sym_ptyp) ) {
			case sp_cend :
				return ;
			default:
				strarg(argt) ;
				fprintf(codefile,".define %s\n",argstr) ;
				break ;
			}
		}
	default :
		while ( getarg(any_ptyp) != sp_cend ) ;
		break ;
	}
}


char    *segname[] = {
	".text",        /* SEGTXT */
	".data",        /* SEGCON */
	".data",        /* SEGROM */
	".bss"          /* SEGBSS */
};
