#include <stb.h>

#ifndef lint
static char rcsid[] = "$Header$";
#endif /* lint */
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
	fprintf(codefile,".data4\t%s\n",str);
}

#ifdef MACH_OPTIONS
static int gdb_flag = 0;

mach_option(s)
	char *s;
{
	if (! strcmp(s, "-gdb")) {
		gdb_flag = 1;
	}
	else {
		error("Unknown flag %s", s);
	}
}
#endif /* MACH_OPTIONS */

mes(mesno)
	word	mesno;
{
	int argt, a1, a2 ;

	switch ( (int)mesno ) {
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
	case ms_stb:
		argt = getarg(str_ptyp | cst_ptyp);
		if (argt == sp_cstx)
			fputs(".symb \"\", ", codefile);
		else {
			fprintf(codefile, ".symb \"%s\", ", str);
			argt = getarg(cst_ptyp);
		}
		a1 = argval;
		argt = getarg(cst_ptyp);
		a2 = argval;
		argt = getarg(cst_ptyp|nof_ptyp|sof_ptyp|ilb_ptyp|pro_ptyp);
#ifdef MACH_OPTIONS
		if (gdb_flag) {
			if (a1 == N_PSYM) {
				/* Change offset from AB into offset from
				   the frame pointer (ab).
				*/
				argval += 4;
			}
		}
#endif
		fprintf(codefile, "%s, 0x%x, %d\n", strarg(argt), a1, a2);
		argt = getarg(end_ptyp);
		break;
	case ms_std:
		argt = getarg(str_ptyp | cst_ptyp);
		if (argt == sp_cstx)
			str[0] = '\0';
		else {
			argt = getarg(cst_ptyp);
		}
		swtxt();
		if (argval == N_SLINE
#ifdef MACH_OPTIONS
		    && ! gdb_flag
#endif
		) {
			fputs("calls $0,___u_LiB\n", codefile);
			cleanregs();	/* debugger might change variables */
		}
		fprintf(codefile, ".symd \"%s\", 0x%x,", str, (int) argval);
		argt = getarg(cst_ptyp);
		fprintf(codefile, "%d\n", (int) argval);
		argt = getarg(end_ptyp);
		break;
	default :
		while ( getarg(any_ptyp) != sp_cend ) ;
		break ;
	}
}

#define PDPFLOAT
#define CODE_GENERATOR
#include <con_float>

#ifndef REGVARS
prolog(nlocals)
	full	nlocals;
{
	fprintf(codefile,".data2 00\n");
	if (nlocals == 0)
		return;
	if (nlocals == 4)
		fprintf(codefile,"\tclrl\t-(sp)\n");
	else if (nlocals == 8)
		fprintf(codefile,"\tclrq\t-(sp)\n");
	else
		fprintf(codefile,"\tsubl2\t$%ld,sp\n",nlocals);
}

#endif /* not REGVARS */

char    *segname[] = {
	".sect .text",	/* SEGTXT */
	".sect .data",	/* SEGCON */
	".sect .rom",	/* SEGROM */
	".sect .bss"	/* SEGBSS */
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
		"\t! Local %ld, size %d, to register %s\n",
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
	fprintf(codefile, ".data2 0%o\n", mask);
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

#endif /* REGVARS */
