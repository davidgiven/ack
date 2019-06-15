/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 *
 */

/*
 * machine dependent back end routines for the Zilog Z80 (as well as Intel 8080)
 */


void con_part(register int sz, word w)
{

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

void con_mult(word sz)
{
	if (argval != 4)
		fatal("bad icon/ucon size");
	fprintf(codefile, ".data4\t%ld\n", atol(str));
}

void con_float(void)
{
	static int been_here;
	int sz = argval;

	if (sz != 4 && sz != 8) {
		fatal("bad fcon size");
	}
	if (! been_here) {
		been_here = 1;
		fprintf(stderr,"warning: dummy floating constant(s)\n");
	}
	while (sz) {
		fprintf(codefile, ".data4 0	!dummy float\n");
		sz -= 4;
	}
}

void prolog(full nlocals)
{
	fprintf(codefile,"\tpush\tiy\n\tld\thl,0\n\tadd\thl,sp\n\tpush\thl\n\tpop\tiy\n");
	switch (nlocals) {
	case 8: fprintf(codefile,"\tpush\thl\n");
	case 6: fprintf(codefile,"\tpush\thl\n");
	case 4: fprintf(codefile,"\tpush\thl\n");
	case 2: fprintf(codefile,"\tpush\thl\n");
	case 0: break;
	default:
		fprintf(codefile,"\tld\thl,%d\n\tadd\thl,sp\n\tld\tsp,hl\n",-nlocals);
		break;
	}
}

void mes(word type)
{
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

char *segname[] = {
	".sect .text",
	".sect .data",
	".sect .rom",
	".sect .bss"
};
