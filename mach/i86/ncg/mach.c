/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 *
 */

#ifndef NORCSID
static char rcs_m[]= "$Id$" ;
static char rcs_mh[]= ID_MH ;
#endif

/*
 * machine dependent back end routines for the Intel 8086
 */

con_part(sz,w) register sz; word w; {

	while (part_size % sz)
		part_size++;
	if (part_size == TEM_WSIZE)
		part_flush();
	if (sz == 1) {
		w &= 0xFF;
		if (part_size)
			w <<= 8;
		part_word |= w;
	} else {
		assert(sz == 2);
		part_word = w;
	}
	part_size += sz;
}

con_mult(sz) word sz; {
	long l;

	if (sz != 4)
		fatal("bad icon/ucon size");
	l = atol(str);
	fprintf(codefile,"\t.data2 %d,%d\n",
			(int)l&0xFFFF,(int)(l>>16)&0xFFFF);
}

#define CODE_GENERATOR 
#define IEEEFLOAT 
#define FL_MSL_AT_LOW_ADDRESS	0
#define FL_MSW_AT_LOW_ADDRESS	0
#define FL_MSB_AT_LOW_ADDRESS	0
#include <con_float>

/*

string holstr(n) word n; {

	sprintf(str,hol_off,n,holno);
	return(mystrcpy(str));
}
*/

#ifdef REGVARS
full lbytes;
#endif

prolog(nlocals) full nlocals; {

	fputs("\tpush\tbp\n\tmov\tbp,sp\n", codefile);
#ifdef REGVARS
	lbytes = nlocals;
#else
	switch (nlocals) {
	case 4: fputs("\tpush\tax\n", codefile);
	case 2: fputs("\tpush\tax\n", codefile);
	case 0: break;
	default:
		fprintf(codefile, "\tsub\tsp,%d\n",nlocals); break;
	}
#endif
}

#ifdef REGVARS
long si_off;
long di_off;
int firstreg;

regscore(off, size, typ, score, totyp)
	long off;
{
	if (size != 2) return -1;
	score -= 1;
	score += score;
	if (typ == reg_pointer || typ == reg_loop) score += (score >> 2);
	score -= 2;	/* cost of saving */
	if (off >= 0) score -= 3;
	return score;
}

i_regsave()
{
	si_off = -1;
	di_off = -1;
	firstreg = 0;
}

f_regsave()
{
	if (si_off != di_off) {
		if (di_off == -lbytes) lbytes -= 2;
		if (si_off == -lbytes) lbytes -= 2;
		if (di_off == -lbytes) lbytes -= 2;
	}
	switch (lbytes) {
	case 4: fputs("\tpush\tax\n", codefile);
	case 2: fputs("\tpush\tax\n", codefile);
	case 0: break;
	default:
		fprintf(codefile, "\tsub\tsp,%d\n",lbytes); break;
	}
	if (firstreg == 1) {
		fputs("push di\n", codefile);
		if (si_off != -1) fputs("push si\n", codefile);
	}
	else if (firstreg == -1) {
		fputs("push si\n", codefile);
		if (di_off != -1) fputs("push di\n", codefile);
	}
	if (di_off >= 0)
		fprintf(codefile, "mov di,%ld(bp)\n", di_off);
	if (si_off >= 0)
		fprintf(codefile, "mov si,%ld(bp)\n", si_off);
}

regsave(regstr, off, size)
	char *regstr;
	long off;
{
	if (strcmp(regstr, "si") == 0) {
		if (! firstreg) firstreg = -1;
		si_off = off;
	}
	else {
		if (! firstreg) firstreg = 1;
		di_off = off;
	}
}

regreturn()
{
	if (firstreg == 1) {
		if (si_off != -1) fputs("jmp .sdret\n", codefile);
		else fputs("jmp .dret\n", codefile);
	}
	else if (firstreg == -1) {
		if (di_off != -1) fputs("jmp .dsret\n", codefile);
		else fputs("jmp .sret\n", codefile);
	}
	else fputs("jmp .cret\n", codefile);
}
#endif /* REGVARS */

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
				fprintf(codefile, ".define %s\n",argstr) ;
				break ;
			}
		}
	default :
		while ( getarg(any_ptyp) != sp_cend ) ;
		break ;
	}
}

char    *segname[] = {
	".sect .text",        /* SEGTXT */
	".sect .data",        /* SEGCON */
	".sect .rom",        /* SEGROM */
	".sect .bss"          /* SEGBSS */
};
