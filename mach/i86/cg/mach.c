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

	i= argval ;
	if (i!= 4 && i!= 8)
		fatal("bad fcon size");
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
