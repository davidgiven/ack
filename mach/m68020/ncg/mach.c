/* $Header$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 *
 */

/*
 * machine dependent back end routines for the Motorola 68000, 68010 or 68020
 */

#include "whichone.h"

con_part(sz,w) register sz; word w; {

	while (part_size % sz)
		part_size++;
	if (part_size == 4)
		part_flush();
	if (sz == 1) {
		w &= 0xFF;
		w <<= 8*(3-part_size);
		part_word |= w;
	} else if (sz == 2) {
		w &= 0xFFFF;
		if (part_size == 0)
			w <<= 16;
		part_word |= w;
	} else {
		assert(sz == 4);
		part_word = w;
	}
	part_size += sz;
}

con_mult(sz) word sz; {

	if (sz != 4)
		fatal("bad icon/ucon size");
	fprintf(codefile,".data4 %s\n",str);
}

#ifdef NOFLOAT
con_float() {

static int been_here;
	if (argval != 4 && argval != 8)
		fatal("bad fcon size");
	fputs(".data4\t", codefile);
	if (argval == 8)
		fputs("0,", codefile);
	fputs("0 !dummy float\n", codefile);
	if ( !been_here++)
	{
	fputs("Warning : dummy float-constant(s)\n", stderr);
	}
}
#else
#define IEEEFLOAT

con_float() {
	double f;
	double atof();
	float fl;
	int i;
#ifndef OWNFLOAT
	double f1;
	double frexp(), modf();
	int j;
	int sign = 0;
	int fraction ;
#else OWNFLOAT
	char *p;
#endif OWNFLOAT

	if (argval!= 4 && argval!= 8)	{
		fprintf(stderr,"float constant size = %d\n",argval);
		fatal("bad fcon size");
	}
	fprintf(codefile,"!float %s sz %d\n", str, argval);
	f = atof(str);
#ifdef OWNFLOAT
	if (argval == 4) {
		fl = f;
		p = (char *) &fl;
	}
	else {
		p = (char *) &f;
	}
	fprintf(codefile, ".data1 0%o", *p++ & 0377);
	for (i = argval-1; i; i--) {
		fprintf(codefile,",0%o", *p++ & 0377);
	}
#else OWNFLOAT
	f = frexp(f, &i);
	if (f < 0) {
		f = -f;
		sign = 1;
	}
	if (f == 0) {
		if (argval == 8) fprintf(codefile, ".data2 0, 0\n");
		fprintf(codefile, ".data2 0, 0\n");
		return;
	}
	while (f < 0.5) {
		f += f;
		i --;
	}
	f = modf(2 * f, &f1); /* hidden bit */
#ifdef IEEEFLOAT
	if (argval == 4) {
#endif IEEEFLOAT
		i = (i + 128) & 0377;
		fraction = (sign << 15) | (i << 7);
		for (j = 6; j>= 0; j--) {
			if (f >= 0.5) fraction |= (1 << j);
			f = modf(2*f, &f1);
		}
#ifdef IEEEFLOAT
	}
	else {
		i = (i + 1024) & 03777;
		fraction = (sign << 15) | (i << 4);
		for (j = 3; j>= 0; j--) {
			if (f >= 0.5) fraction |= (1 << j);
			f = modf(2*f, &f1);
		}
	}
#endif IEEEFLOAT
	fprintf(codefile, ".data1 0%o, 0%o", (fraction>>8)&0377, fraction&0377);
	for (i = argval / 2 - 1; i; i--) {
		fraction = 0;
		for (j = 15; j>= 0; j--) {
			if (f >= 0.5) fraction |= (1 << j);
			f = modf(2*f, &f1);
		}
		fprintf(codefile, ", 0%o, 0%o", (fraction>>8)&0377, fraction&0377);
	}
#endif OWNFLOAT
	putc('\n', codefile);
}
#endif

#define MOVEM_LIMIT	2
/* If #registers to be saved exceeds MOVEM_LIMIT, we
* use the movem instruction to save registers; else
* we simply use several move.l's.
*/


regscore(off,size,typ,score,totyp)
	long off;
{
	if (score == 0) return -1;
	switch(typ) {
		case reg_float:
			return -1;
		case reg_pointer:
			if (size != 4 || totyp != reg_pointer) return -1;
			score += (score >> 1);
			break;
		case reg_loop:
			score += 5;
			/* fall through .. */
		case reg_any:
			if (size != 4 || totyp == reg_pointer) return -1;
			break;
	}
	if (off >= 0) {
		/* parameters must be initialised with an instruction
		 * like "move.l 4(a6),d0", which costs 2 words.
		 */
		score -= 2;
	}
	score--;	/* save/restore */
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

save()
{
	register struct regsav_t *p;

	if (regnr > MOVEM_LIMIT) {
		fputs("movem.l ", codefile);
		for (p = regsav; ;) {
			fputs(p->rs_reg, codefile);
			if (++p == &regsav[regnr]) break;
			putc('/',codefile);
		}
		fputs(",-(sp)\n", codefile);
	} else {
		for (p = regsav; p < &regsav[regnr]; p++) {
			fprintf(codefile,"move.l %s,-(sp)\n",p->rs_reg);
		}
	}
	/* initialise register-parameters */
	for (p = regsav; p < &regsav[regnr]; p++) {
		if (p->rs_off >= 0) {
#ifdef TBL68020
			fprintf(codefile,"move.%c (%ld,a6),%s\n",
#else
			fprintf(codefile,"move.%c %ld(a6),%s\n",
#endif
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
		fputs("movem.l (sp)+,", codefile);
		for (p = regsav; ;) {
			fputs(p->rs_reg, codefile);
			if (++p == &regsav[regnr]) break;
			putc('/',codefile);
		}
		putc('\n',codefile);
	} else {
		for (p = &regsav[regnr-1]; p >= regsav; p--) {
			fprintf(codefile,"move.l (sp)+,%s\n",p->rs_reg);
		}
	}
	fputs("unlk a6\nrts\n", codefile);
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


prolog(nlocals) full nlocals; {

#ifdef TBL68020
	fprintf(codefile,"link\ta6,#-%ld\n",nlocals);
#else
	fprintf(codefile,"tst.b -%ld(sp)\nlink\ta6,#-%ld\n",nlocals+40,nlocals);
#endif
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
	".sect .text",	/* SEGTXT */
	".sect .data",	/* SEGCON */
	".sect .rom",	/* SEGROM */
	".sect .bss"	/* SEGBSS */
};
