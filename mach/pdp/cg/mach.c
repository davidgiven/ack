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

/* #define REGPATCH		/* save all registers in markblock */

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
	long l, atol();

	if (sz != 4)
		fatal("bad icon/ucon size");
#ifdef ACK_ASS
	fprintf(codefile,".data4 %s\n",str);
#else
	l = atol(str);
	fprintf(codefile,"\t%o;%o\n",(int)(l>>16),(int)l);
#endif
}

/*
 * The next function is difficult to do when not running on a PDP 11 or VAX
 * The strategy followed is to assume the code generator is running on a PDP 11
 * unless the ACK_ASS define  is on.
 * In the last case floating point constants are simply not handled
 */

con_float() {
#ifdef ACK_ASS
	static int been_here;

	if (argval != 4 && argval != 8)
		fatal("bad fcon size");
	fprintf(codefile,".data4\t");
	if (argval == 8)
		fprintf(codefile,"F_DUM,");
	fprintf(codefile,"F_DUM\n");
	if ( !been_here++)
		fprintf(stderr,"Warning : dummy float-constant(s)\n");
#else
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
#endif
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

	Rstring[0] = 0;
	n_regvars=0;
}

f_regsave() {
	register i;

	if (n_regvars==0 || lbytes==0) {
#ifdef REGPATCH
		fprintf(codefile,"mov r2,-(sp)\nmov r4,-(sp)\n");
#endif
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

	fprintf(codefile,"%c Local %ld into %s\n",COMMENTCHAR,off,regstr);
/* commented away 
#ifndef REGPATCH
	fprintf(codefile,"mov %s,-(sp)\n",regstr);
#endif
	strcat(Rstring,regstr);
	if (off>=0)
		fprintf(codefile,"mov 0%lo(r5),%s\n",off,regstr);
end of commented away */

	strcat(Rstring,regstr);
	regadm[n_regvars].ra_str = regstr;
	regadm[n_regvars].ra_off = off;
	n_regvars++;
}

regreturn() {

#ifdef REGPATCH
	fprintf(codefile,"jmp eret\n");
#else
	fprintf(codefile,"jmp RT%s\n",Rstring);
#endif
}

#endif

prolog(nlocals) full nlocals; {

#ifndef REGVARS
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
#ifdef ACK_ASS
				fprintf(codefile,".define %s\n",argstr) ;
#else
				fprintf(codefile,".globl %s\n",argstr) ;
#endif
				break ;
			}
		}
	default :
		while ( getarg(any_ptyp) != sp_cend ) ;
		break ;
	}
}

char    *segname[] = {
#ifdef ACK_ASS
	".sect .text",        /* SEGTXT */
	".sect .data",        /* SEGCON */
	".sect .rom",         /* SEGROM */
	".sect .bss"          /* SEGBSS */
#else
	".text",        /* SEGTXT */
	".data",        /* SEGCON */
	".data",        /* SEGROM */
	".bss"          /* SEGBSS */
#endif
};
