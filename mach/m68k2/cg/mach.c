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

con_part(sz,w) register sz; word w; {

	while (part_size % sz)
		part_size++;
	if (part_size == EM_WSIZE)
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

con_float() {

static int been_here;
	if (argval != 4 && argval != 8)
		fatal("bad fcon size");
	fprintf(codefile,".long\t");
	if (argval == 8)
		fprintf(codefile,"F_DUM,");
	fprintf(codefile,"F_DUM\n");
	if ( !been_here++)
	{
	fprintf(stderr,"Warning : dummy float-constant(s)\n");
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
