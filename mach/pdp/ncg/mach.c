#ifndef NORCSID
static char rcsid[] = "$Header$";
#endif

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

	if (sz != 4)
		fatal("bad icon/ucon size");
	l = atol(str);
	fprintf(codefile,"\t%o;%o\n",(int)(l>>16),(int)l);
}

con_float() {
	double f;
	register short *p,i;

	/*
	 * This code is correct only when the code generator is
	 * run on a PDP-11 or VAX-11 since it assumes native
	 * floating point format is PDP-11 format.
	 */

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

char Rstring[10];
full lbytes;
struct regadm {
	char *ra_str;
	long  ra_off;
} regadm[2];
int n_regvars;

regscore(off,size,typ,score,totyp) long off; {

	/*
	 * This function is full of magic constants.
	 * They are a result of experimentation.
	 */

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
	return(score);	/* 10 * estimated # of words of profit */
}

i_regsave() {

	Rstring[0] = 0;
	n_regvars=0;
}

f_regsave() {
	register i;

	if (n_regvars==0 || lbytes==0) {
		fprintf(codefile,"mov r5,-(sp)\nmov sp,r5\n");
		if (lbytes == 2)
			fprintf(codefile,"tst -(sp)\n");
		else if (lbytes!=0)
			fprintf(codefile,"sub $0%o,sp\n",lbytes);
		for (i=0;i<n_regvars;i++)
			fprintf(codefile,"mov %s,-(sp)\n",regadm[i].ra_str);
	} else {
		if (lbytes>6) {
			fprintf(codefile,"mov $0%o,r0\n",lbytes);
			fprintf(codefile,"jsr r5,PR%s\n",Rstring);
		} else {
			fprintf(codefile,"jsr r5,PR%d%s\n",lbytes,Rstring);
		}
	}
	for (i=0;i<n_regvars;i++)
		if (regadm[i].ra_off>=0)
			fprintf(codefile,"mov 0%lo(r5),%s\n",regadm[i].ra_off,
						regadm[i].ra_str);
}

regsave(regstr,off,size) char *regstr; long off; {

	fprintf(codefile,"/ Local %ld into %s\n",off,regstr);
	strcat(Rstring,regstr);
	regadm[n_regvars].ra_str = regstr;
	regadm[n_regvars].ra_off = off;
	n_regvars++;
}

regreturn() {

	fprintf(codefile,"jmp RT%s\n",Rstring);
}

#endif

prolog(nlocals) full nlocals; {

#ifndef REGVARS
	fprintf(codefile,"mov r5,-(sp)\nmov sp,r5\n");
	if (nlocals == 0)
		return;
	if (nlocals == 2)
		fprintf(codefile,"tst -(sp)\n");
	else
		fprintf(codefile,"sub $0%o,sp\n",nlocals);
#else
	lbytes = nlocals;
#endif
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
