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

#define IEEEFLOAT
#define CODE_GENERATOR
#define FL_MSL_AT_LOW_ADDRESS	1
#define FL_MSW_AT_LOW_ADDRESS	1
#define FL_MSB_AT_LOW_ADDRESS	1
#include <con_float>

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

full nlocals;

regreturn()
{
	register struct regsav_t *p;

	if (regnr > 1)  {
#ifdef TBL68020
		fprintf(codefile,"movem.l (-%ld,a6),", nlocals);
#else
		fprintf(codefile,"movem.l -%ld(a6),", nlocals);
#endif
		for (p = regsav; ;) {
			fputs(p->rs_reg, codefile);
			if (++p == &regsav[regnr]) break;
			putc('/',codefile);
		}
		putc('\n',codefile);
	} else if (regnr == 1) {
		p = regsav;
#ifdef TBL68020
		fprintf(codefile,"move.l (-%ld,a6),%s\n",nlocals, p->rs_reg);
#else
		fprintf(codefile,"move.l -%ld(a6),%s\n",nlocals, p->rs_reg);
#endif
	}
	fputs("unlk a6\nrts\n", codefile);
}

f_regsave()
{
	register struct regsav_t *p;

	nlocals += regnr*4;
#ifdef TBL68020
	fprintf(codefile,"link\ta6,#-%ld\n",nlocals);
#else
	fprintf(codefile,"tst.b -%ld(sp)\nlink\ta6,#-%ld\n",nlocals+40,nlocals);
#endif
	if (regnr > 1) {
		fputs("movem.l ", codefile);
		for (p = regsav; ;) {
			fputs(p->rs_reg, codefile);
			if (++p == &regsav[regnr]) break;
			putc('/',codefile);
		}
		fputs(",(sp)\n", codefile);
	} else if (regnr == 1) {
		p = regsav;
		fprintf(codefile,"move.l %s,(sp)\n",p->rs_reg);
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

regsave(s,off,size)
	char *s;
	long off;
{
	assert (regnr < 9);
	regsav[regnr].rs_reg = s;
	regsav[regnr].rs_off = off;
	regsav[regnr++].rs_size = size;
	fprintf(codefile, "!Local %ld into %s\n",off,s);
}

prolog(n) full n; {

	nlocals = n;
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
