/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 *
 */

#ifndef NORCSID
static char rcsid[]=	"$Id$" ;
#endif

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

long atol();

con_mult(sz) word sz; {

	if (argval != 4)
		fatal("bad icon/ucon size");
	fprintf(codefile,".data4\t%ld\n",atol(str));
}

con_float() {
	static int warning_given;
	int i = argval;

	if (!warning_given) {
		fprintf(stderr, "warning: dummy floating point constant\n");
		warning_given = 1;
	}
	while (i > 0) {
		fputs(".data4 0	!dummy float\n", codefile);
		i -= 4;
	}
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
	".sect .text",
	".sect .data",
	".sect .rom",
	".sect .bss"
};
