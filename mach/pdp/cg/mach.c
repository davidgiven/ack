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
 * Author: Hans van Staveren
 */

/*
 * machine dependent back end routines for the PDP-11
 */

#define REGPATCH

con_part(sz,w) register sz; word w; {

	while (part_size % sz)
		part_size++;
	if (part_size == EM_WSIZE)
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
	fprintf(codefile,"\t%o;%o\n",(int)(l>>16),(int)l);
}

con_float() {
	double f;
	register short *p,i;

	if (argval != 4 && argval != 8)
		fatal("bad fcon size");
	f = atof(str);
	p = (short *) &f;
	i = *p++;
	if (argval == 8) {
		fprintf(codefile,"\t%o;%o;",i,*p++);
		i = *p++;
	}
	fprintf(codefile,"\t%o;%o\n",i,*p++);
}

#ifdef REGVARS

char Rstring[10] = "RT";

regscore(off,size,typ,score,totyp) long off; {

	if (size != 2)
		return(-1);
	score -= 1;	/* allow for save/restore */
	if (off>=0)
		score -= 2;
	if (typ==reg_pointer)
		score *= 17;
	else if (typ==reg_loop)
		score = 10*score+50;	/* Guestimate */
	else
		score *= 10;
	return(score);	/* estimated # of words of profit */
}

i_regsave() {

	Rstring[2] = 0;
}

f_regsave() {}

regsave(regstr,off,size) char *regstr; long off; {

	fprintf(codefile,"/ Local %ld into %s\n",off,regstr);
#ifndef REGPATCH
	fprintf(codefile,"mov %s,-(sp)\n",regstr);
#endif
	strcat(Rstring,regstr);
	if (off>=0)
		fprintf(codefile,"mov 0%lo(r5),%s\n",off,regstr);
}

regreturn() {

#ifdef REGPATCH
	fprintf(codefile,"jmp eret\n");
#else
	fprintf(codefile,"jmp %s\n",Rstring);
#endif
}

#endif

prolog(nlocals) full nlocals; {

#ifdef REGPATCH
	fprintf(codefile,"mov r2,-(sp)\nmov r4,-(sp)\n");
#endif
	fprintf(codefile,"mov r5,-(sp)\nmov sp,r5\n");
	if (nlocals == 0)
		return;
	if (nlocals == 2)
		fprintf(codefile,"tst -(sp)\n");
	else
		fprintf(codefile,"sub $0%o,sp\n",nlocals);
}

dlbdlb(as,ls) string as,ls; {

	if (strlen(as)+strlen(ls)+2<sizeof(labstr)) {
		strcat(ls,":");
		strcat(ls,as);
	} else
		fatal("too many consecutive labels");
}

mes(type) word type; {
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
				fprintf(codefile,".globl %s\n",argstr) ;
				break ;
			}
		}
	default :
		while ( getarg(any_ptyp) != sp_cend ) ;
		break ;
	}
}

char    *segname[] = {
	".text",        /* SEGTXT */
	".data",        /* SEGCON */
	".data",        /* SEGROM */
	".bss"          /* SEGBSS */
};
