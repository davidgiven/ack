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
 * machine dependent back end routines for the Intel 8080.
 */

con_part(sz,w) register sz; word w; {

	while (part_size % sz)
		part_size++;
	if (part_size == 2)
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

	fprintf(codefile,"\tpush\tb\n\tlxi\th,0\n\tdad\tsp\n\tmov\tb,h\n\tmov\tc,l\n");
	switch (nlocals) {
	case 4: fprintf(codefile,"\tpush\th\n");
	case 2: fprintf(codefile,"\tpush\th\n");
	case 0: break;
	default:
		fprintf(codefile,"\tlxi\th,%d\n\tdad\tsp\n\tsphl\n",-nlocals);
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
