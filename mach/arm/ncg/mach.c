/* mach.c file for ARM backend */


#define MAXREGVARS	4		/* Max num of register variables*/

struct reg_stuff
{
    char *name;
    long offset;
}
reg [MAXREGVARS];			/* array of reg var details	*/

char regnames[6*MAXREGVARS];		/* comma-delimited list of regs	*/
int  n_regs;				/* current number of reg vars	*/

char *segname[] = {
	".sect .text",
	".sect .data",
	".sect .data",
	".sect .bss"
};

con_part(sz, w)
	register int	sz;
	word		w;
{
	while (part_size % sz) part_size++;

	if (part_size == TEM_WSIZE)
		part_flush();
	if (sz == 1 || sz == 2) {
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


regscore (offset, size, type, score, totype)
long offset;
{
    if (size > 4)			/* only consider register-sized	*/
	return -1;			/* variables			*/

    if (score < 2) return -1;

    /* give greater preference to locals */
    if (offset >= 0) score *= 4; else score *= 8;

    return score;
}


i_regsave()
{
    n_regs = 0;
    *regnames = '\0';
}


regsave(name, offset, size)
char *name;
long offset;
{

    if (n_regs > 0)			/* if its not the first item	*/
	strcat(regnames, ",");		/* prefix it with a comma	*/
    strcat(regnames, name);		/* add to list of names to save */

    reg[n_regs].name = name;		/* and add to array of reg vars	*/
    reg[n_regs++].offset = offset;
}


f_regsave()
{
    int i;
    long n;
    char *q;

    if (n_regs > 0)
	fprintf(codefile, "STMFD R12<,{%s}\n", regnames);

    for (i = 0; i < n_regs; i++) {	/* load their new values	*/
	n = reg[i].offset;
	q = reg[i].name;
    if (n > 0)				/* only proc parameters		*/
	    fprintf(codefile, "LDR %s,[R13,#%ld]\n", q, n);
    }
}


regreturn()
{
      if (n_regs > 0)
	fprintf(codefile, "LDMFD R12<,{%s}\n", regnames);
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

prolog(nlocals) full nlocals;
{
	fprintf(codefile, "STMFD R12<,{R13,R14}\n");
	fprintf(codefile, "MOV R13,R12\n");
	if (nlocals)
		fprintf(codefile, "SUB R12,R12,#%d\n", nlocals);
	return;
}

#define CODE_GENERATOR
#define IEEEFLOAT
#define FL_MSL_AT_LOW_ADDRESS   0
#define FL_MSW_AT_LOW_ADDRESS   0
#define FL_MSB_AT_LOW_ADDRESS   0
#include <con_float>

