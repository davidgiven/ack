/* $Id$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
void
con_part(int sz, word w) {

	while (part_size % sz)
		part_size++;
	if (part_size == TEM_WSIZE)
		part_flush();
	if (sz == 1) {
		w &= 0xFF;
		if (part_size == 0) {
			/* Shift 8 for m68k2, 16 otherwise */
			w <<= 8;
		}
		part_word |= w;
	} else {
if (sz!=2)
  fprintf(stderr,"con_part(%d,%04x)\n",sz,w);
		assert(sz == 2);
		part_word = w;
	}
	part_size += sz;
}

void con_mult(word sz)
{
	if (sz != 4)
		fatal("bad icon/ucon size");
	fprintf(codefile,".data4 %ld\n", atol(str));
}


void con_float(void)
{
static int been_here;
	if (argval != 4 && argval != 8)
		fatal("bad fcon size");
	fprintf(codefile,".data4\t");
	if (argval == 8)
		fprintf(codefile,"F_DUM,");
	fprintf(codefile,"F_DUM\n");
	if ( !been_here++)
	{
	fprintf(stderr,"Warning : dummy float-constant(s)\n");
	}
}

void prolog(full nlocals)
{
	int i;

	fprintf(codefile,"\tjsr Pro\n");
	fprintf(codefile,"! nlocals %d\n",nlocals);
	if (nlocals == 0)
		return;
	else if (nlocals < 8)
		for (i=0; i<nlocals; i++) {
			fprintf(codefile,"\tdes\n");
		}
	else
		fprintf(codefile,
	"\tldab #[%d].l\n\tldaa #[%d].h\n\tjsr Lcs\n",
					-nlocals, -nlocals);
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

char    *segname[] = {
	".sect .text",        /* 1:SEGTXT */
/*	".sect .rom",        /* 2:SEGROM */
	".sect .data",        /* 2:SEGROM */
	".sect .data",        /* 3:SEGCON */
	".sect .bss",          /* 4:SEGBSS */
	".sect .end",		/* 5:SEGEND */
	".sect .zero",		/* 0:SEGZERO */
};
