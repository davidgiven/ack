#ifndef lint
static char rcsid[] = "$Header$";
#endif lint
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 *
 */

/*
 * Machine dependent back end routines for the VAX using 4-byte words
 */

/*
 * Byte order: |  3  |  2  |  1  |  0  |
 */
con_part(sz, w)
	register int	sz;
	word		w;
{
	/*
	 * Align new bytes on boundary of its on size.
	 */
	while (part_size % sz) part_size++;

	if (part_size == TEM_WSIZE)
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
}

con_mult(sz)
	word	sz;
{
	if (sz != 4)
		fatal("bad icon/ucon size");
	fprintf(codefile,".long\t%s\n",str);
}

mes(mesno)
	word	mesno;
{
	while (getarg(any_ptyp) != sp_cend );
}

con_float()
{
	/*
	 * Insert a dot at the right position, if it is not present,
	 * to make the floating point constant acceptable to the assembler.
	 */
	register char	*c;
	extern char	*index();

	if (argval != 4 && argval != 8)
		fatal("bad fcon size");
	if (argval == 8)
		fprintf(codefile,".double\t0d");
	else	fprintf(codefile,".float\t0f");

	if (index(str,'.') != (char *) 0) {
		fprintf(codefile,"%s\n",str);
	
	/*
	 * There must be a dot after the `e' or - if the `e' is not present -
	 * at the end.
	 */
	} else if ((c = index(str,'e')) != (char *) 0) {
		*c++ = '\0';
		fprintf(codefile,"%s.e%s\n",str,c--);
		*c = 'e';
	} else	fprintf(codefile,"%s.\n",str);
}

#ifndef REGVARS
prolog(nlocals)
	full	nlocals;
{
	fprintf(codefile,".word 00\n");
	if (nlocals == 0)
		return;
	if (nlocals == 4)
		fprintf(codefile,"\tclrl\t-(sp)\n");
	else if (nlocals == 8)
		fprintf(codefile,"\tclrq\t-(sp)\n");
	else
		fprintf(codefile,"\tsubl2\t$%ld,sp\n",nlocals);
}

#endif not REGVARS

char    *segname[] = {
	".text",	/* SEGTXT */
	".data",	/* SEGCON */
	".data",	/* SEGROM */
	".data"		/* SEGBSS */
};

#ifdef REGVARS
static full	nlocals;	/* Number of local variables. */

#define NR_REG		8	/* Number of registers. */
#define FIRST_REG	4
#define LAST_REG	(FIRST_REG + NR_REG - 1)

/*
 * Save number of locals.
 */
prolog(n)
{
	nlocals = n;
}

/*
 * Structure to store information about the registers to be stored.
 */
static struct s_reg {
	char	*sr_str;	/* Name of register used. */
	long	sr_off;		/* Offset from LB. */
	int	sr_size;	/* Size in bytes. */
} a_reg[NR_REG + 1], *p_reg;

/*
 * Initialize saving of registers.
 */
i_regsave()
{
	p_reg = a_reg;
}

/*
 * Called for each register to be saved.
 * Save the parameters in the struct.
 */
regsave(str, off, size)
	char	*str;
	long	off;
	int	size;
{
	p_reg->sr_str = str;
	p_reg->sr_off = off;
	(p_reg++)->sr_size = size;
	fprintf(codefile,
		"\t# Local %ld, size %d, to register %s\n",
		off, size, str
	);
}

/*
 * Generate code to save the registers.
 */
f_regsave()
{
	register struct s_reg	*p;
	register int	mask;
	register int	i;
	register int	count;

	mask = 0;
	count = p_reg - a_reg;
	/*
	 * For each register to be saved, set a bit in the
	 * mask corresponding to its number.
	 */
	for (p = a_reg; p < p_reg; p++) {
		i = atoi(p->sr_str + 1);
		if (p->sr_size <= 4)
			mask |= (1 << i);
		else {
			mask |= (3 << i);
			count++;
		}
	}
	/*
	 * Now generate code to save registers.
	 */
	fprintf(codefile, ".word 0%o\n", mask);
	/*
	 * Emit code to initialize parameters in registers.
	 */
	for (p = a_reg; p < p_reg; p++)
		if (p->sr_off >= 0)
			fprintf(codefile,
				"mov%c\t%ld(ap), %s\n",
				p->sr_size == 4 ? 'l' : 'q',
				p->sr_off,
				p->sr_str
			);

	/*
	 * Generate room for locals.
	 */
	if (nlocals == 0)
		return;
	if (nlocals == 4)
		fprintf(codefile,"clrl\t-(sp)\n");
	else if (nlocals == 8)
		fprintf(codefile,"clrq\t-(sp)\n");
	else
		fprintf(codefile,"subl2\t$%ld,sp\n",nlocals);

}

regreturn() { }

regscore(off, size, typ, score, totyp)
	long	off;
	int	size, typ, totyp, score;
{
	register int	i = score;

	/*
	 * If the offset doesn't fit in a byte, word-offset is used,
	 * which is one byte more expensive.
	 */
	if (off > 127 || off < -128) i *= 2;
	/*
	 * Compute cost of initialization for parameters.
	 */
	if (off > 127) i -= 5;
	else if (off >= 0) i -= 4;
	/*
	 * Storing a pointer in a register sometimes saves an instruction.
	 */
	if (typ == reg_pointer) i += score;
	else if (typ == reg_loop) i += 5;
	/*
	 * Now adjust for the saving of the register.
	 * But this costs no space at all.
	 */
	return i - 1;
}

#endif REGVARS
