/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
#define RCSID5 "$Id$"

/*
 * Signetics 2650 special routines
 */

#define MASK	0x6000
#define MASK1	0x1FFF
#define MASK2	0x1FFE

void page(void)
{
	if(((DOTVAL - 1) & MASK1) == MASK1)
		serror("page violation");
}

void pag2(void)
{
	if(((DOTVAL - 1) & MASK1) >= MASK2)
		serror("page violation");
}

void rela(expr_t exp,int ind)
{
	int c, d;
	page();
	if(pass == PASS_3){
#ifdef ASLD
		if((DOTVAL & MASK) != (exp.val & MASK))
			serror("relative address outside page");
#endif
		d = low13(DOTVAL);
		c = low13(exp.val);
#ifdef ASLD
		/*
		 * I (Duk Bekema) think the next two if's are meant to
		 * handle wrap around, but the original author didn't give
		 * any comment here and I don't know anything about the
		 * Signetics 2650.
		 */
		if(c - d > 0x1F80)
			d |= 0x2000;
		if(d - c > 0x1F80)
			c |= 0x2000;
#endif
		c -= d + 1;
		fit(fit7(c));
		}
#ifdef RELOCATION
	newrelo(exp.typ, RELPC | RELO1);
#endif
	emit1(low7(c) | ind);
}

void absoa(expr_t exp, int ix, unsigned int ind)
{
	pag2();
#ifdef ASLD
	if(pass == PASS_3 && (exp.val & MASK) != (DOTVAL & MASK))
		serror("page violation");
#endif
#ifdef RELOCATION
	newrelo(exp.typ, RELO2 | RELBR);
#endif
	emit2(low13(exp.val) | ind | ix);
}

void absob(expr_t exp, unsigned int ind)
{
	pag2();
#ifdef ASLD
	if (exp.val < 0)
		serror("branch to nonexistent memory");
#endif
#ifdef RELOCATION
	newrelo(exp.typ, RELO2 | RELBR);
#endif
	emit2(exp.val | ind);
}
