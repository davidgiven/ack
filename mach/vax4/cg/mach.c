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

/* Machine dependent back end routines for the VAX using 4-byte words
 */

con_part(sz,w) register sz; word w; {

	/* Byte order: |  3  |  2  |  1  |  0  | */

	/* Align new bytes on boundary of its on size.  */
	while (part_size % sz) part_size++;

	if (part_size == EM_WSIZE)
		part_flush();
	if (sz == 1 || sz == 2) {
		/* Smaller than a machineword. */
		w &= (sz == 1 ? 0xFF : 0xFFFF);
		w <<= 8 * part_size;
		part_word |= w;
	} else {
		assert(sz == 4);
		part_word = w;
	}
	part_size += sz;
} /* con_part */

con_mult(sz) word sz; {

	if (sz != 4)
		fatal("bad icon/ucon size");
	fprintf(codefile,".long\t%s\n",str);
}

mes(mesno) word mesno ; {
	while (getarg(any_ptyp) != sp_cend );
}

con_float() {

	/* Insert a dot at the right position, if it is not present,
	 * to make the floating point constant acceptable to the assembler.
	 */
	register char *	c;
	extern char *	index();

	if (argval != 4 && argval != 8)
		fatal("bad fcon size");
	if (argval == 8)
		fprintf(codefile,".double\t0d");
	else	fprintf(codefile,".float\t0f");

	if (index(str,'.') != (char *) 0) {
		fprintf(codefile,"%s\n",str);
	
	/* There must be a dot after the `e' or - if the `e' is not present -
	 * at the end.
	 */
	} else if ((c = index(str,'e')) != (char *) 0) {
		*c++ = '\0';
		fprintf(codefile,"%s.e%s\n",str,c--);
		*c = 'e';
	} else	fprintf(codefile,"%s.\n",str);
} /* con_float */

#ifndef REGVARS
prolog(nlocals) full nlocals; {

	fprintf(codefile,"\tpushl\tfp\n\tmovl\tsp,fp\n");
	if (nlocals == 0)
		return;
	if (nlocals == 4)
		fprintf(codefile,"\tclrl\t-(sp)\n");
	else if (nlocals == 8)
		fprintf(codefile,"\tclrq\t-(sp)\n");
	else
		fprintf(codefile,"\tsubl2\t$%ld,sp\n",nlocals);
}

#endif REGVARS

char    *segname[] = {
	".text",	/* SEGTXT */
	".data",	/* SEGCON */
	".data",	/* SEGROM */
	".data"		/* SEGBSS */
};

#ifdef REGVARS
int		EM_BSIZE;	/* Difference between AB and LB. */
static int	nlocals;	/* Number of local variables. */

#define NR_REG		8	/* Number of registers. */
#define FIRST_REG	4
#define LAST_REG	(FIRST_REG + NR_REG - 1)

prolog(n) { /* Save number of locals. */
	nlocals = n;
}

/*
 * Structure to store information about the registers to be stored.
 */
static struct s_reg {
	char *	sr_str;	/* Name of register used.	*/
	long	sr_off;	/* Offset from LB.		*/
	int	sr_size;/* Size in bytes.		*/
} a_reg[NR_REG + 1], *p_reg;

i_regsave() { /* Initialize saving of registers. */
	EM_BSIZE = 0;
	p_reg = a_reg;
}

regsave(str, off, size) char * str; long off; int size; {

	/* Called for each register to be saved. */

	p_reg->sr_str = str;
	p_reg->sr_off = off;
	(p_reg++)->sr_size = size;
	fprintf(codefile,
		"\t# Local %ld, size %d, to register %s\n",
		off, size, str
	       );
}

f_regsave() {

	register struct s_reg *	p;
	register int	mask;
	register int	i;
	register int	count;

	mask = 0;
	count = p_reg - a_reg;
	for (p = a_reg; p < p_reg; p++) {
		/* For each register to be saved, set a bit in the
		 * mask corresponding to its number.
		 */
		i = atoi(p->sr_str + 1);
		if (p->sr_size <= 4)
			mask |= (1 << i);
		else {
			mask |= (3 << i);
			count++;
		}
	}

	/* Generate code to save registers. */
	if (count > 2)
		fprintf(codefile, "pushr\t$%d\n", mask);
	else
		for (i = FIRST_REG; i <= LAST_REG; i++) {
			/* For all registers that can be used,
			 * see if it must be saved, and do that as cheap
			 * as possible.
			 */
			if (((mask >> i) & 03) == 03) {
				fprintf(codefile, "movq\tr%d,-(sp)\n",i);
				break;
			} else if ((1 << i) & mask) {
				if (count == 1)
					fprintf(codefile,"pushl\tr%d\n",i);
				else
					fprintf(codefile,"pushr\t$%d\n",mask);
				break;
			}
		}

	/* Save a word indicating which registers were saved.
	 * The count uniquely specifies which registers were saved, because
	 * registers are always allocated consecutively within a class,
	 * starting from FIRST_REG. I only have one class.
	 */
	fprintf(codefile, "pushl\t$%d\n", count);

	/* Compute AB - LB. */
	EM_BSIZE = 4 * count + 12;

	/* Now finish the procedure prolog. */
	fprintf(codefile, "pushl\tfp\nmovl\tsp,fp\n");

	/* Emit code to initialize parameters in registers. */
	for (p = a_reg; p < p_reg; p++) {
		if (p->sr_off >= 0) {
			if (p->sr_size == 4) {
				fprintf(codefile,
					"movl\t%ld(fp),%s\n",
					p->sr_off + EM_BSIZE, p->sr_str
				       );
			} else if (p->sr_size == 8) {
				fprintf(codefile,
					"movq\t%ld(fp),%s\n",
					p->sr_off + EM_BSIZE, p->sr_str
				       );
			}
		}
	}

	/* Generate room for locals. */
	if (nlocals == 0)
		return;
	if (nlocals == 4)
		fprintf(codefile,"clrl\t-(sp)\n");
	else if (nlocals == 8)
		fprintf(codefile,"clrq\t-(sp)\n");
	else
		fprintf(codefile,"subl2\t$%ld,sp\n",nlocals);

} /* f_regsave */

regreturn() {

	fprintf(codefile, "jmp\t.return\n");
} /* regreturn */

regscore(off, size, typ, score, totyp) long off; {

	register int	i;

	i = score;	/* Local/parameter is used score times. */
	
	/* If the offset doesn't fit in a byte, word-offset is used,
	 * which is one byte more expensive.
	 */
	if (off > 109 || off < -128) i *= 2;	/* Word offset. */
	
	/* 109: guestimate. We don't know AB -LB yet.
	 * 109 is the number if there are two registervariables.
	 */

	/* Compute cost of initialization for parameters. */
	if (off > 109)
		i -= 5;
	else if (off >= 0)
		i -= 4;

	if (typ == reg_pointer)
		i += score;	/* Sometimes saves an instruction. */
	else if (typ == reg_loop)
		i += 5;

	i -= 2;	/* Cost of save. */

	/* Actually the first registers are more expensive, but then
	 * the following registers are cheaper.
	 */

	return (i);
} /* regscore */

#endif REGVARS
