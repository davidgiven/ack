/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 *
 */

#ifndef NORCSID
static char rcs_m[]= "$Header$" ;
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

con_float() {
	register i;
	static int warning_given;

	i= argval ;
	if (i!= 4 && i!= 8)
		fatal("bad fcon size");
	if (! warning_given) {
		fprintf(stderr, "Warning: dummy floating point constant(s)\n");
		warning_given = 1;
	}
	while ( i ) {
		fprintf(codefile," .data2 0,0\n") ;
		i -=4 ;
	}
}

/*

string holstr(n) word n; {

	sprintf(str,hol_off,n,holno);
	return(mystrcpy(str));
}
*/

prolog(nlocals) full nlocals; {

	fprintf(codefile,"\tpush\tbp\n\tmov\tbp,sp\n");
	switch (nlocals) {
	case 4: printf("\tpush\tax\n");
	case 2: printf("\tpush\tax\n");
	case 0: break;
	default:
		printf("\tsub\tsp,%d\n",nlocals); break;
	}
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
}

regsave(regstr, off, size)
	char *regstr;
	long off;
{
	if (strcmp(regstr, "si") == 0) {
		if (! firstreg) firstreg = -1;
		si_off = off;
		fputs("push si\n", codefile);
		if (off >= 0)
			fprintf(codefile, "mov si,%ld(bp)\n", off);
	}
	else {
		if (! firstreg) firstreg = 1;
		di_off = off;
		fputs("push di\n", codefile);
		if (off >= 0)
			fprintf(codefile, "mov di,%ld(bp)\n", off);
	}
}

regreturn()
{
	if (firstreg == 1) {
		if (si_off != -1) fputs("pop si\n", codefile);
		fputs("pop di\n", codefile);
	}
	else if (firstreg == -1) {
		if (di_off != -1) fputs("pop di\n", codefile);
		fputs("pop si\n", codefile);
	}
	fputs("mov sp,bp\npop bp\nret\n", codefile);
}
#endif REGVARS

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
				printf(".define %s\n",argstr) ;
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
