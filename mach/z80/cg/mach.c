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

#ifndef NORCSID
static char rcs_mach[] = "$Header$" ;
static char rcs_h[] = RCS_H ;
#endif

/*
 * machine dependent back end routines for the Zilog Z80 (as well as Intel 8080)
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

	if (argval != 4)
		fatal("bad icon/ucon size");
	l = atol(str);
	fprintf(codefile,".short\t%d\n",(int)l);
	fprintf(codefile,".short\t%d\n",(int)(l>>16));
}

con_float() {

	fatal("no reals");
}


prolog(nlocals) full nlocals; {

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

char *segname[] = {
	".text",
	".data",
	".data",
	".bss"
};
