/* mach.c file for ARM backend */


#define MAXREGVARS	4		/* Max num of register variables*/

struct reg_stuff
{
    char *name;
    long offset;
}
reg [MAXREGVARS];			/* array of reg var details	*/

char regnames[6*MAXREGVARS];		/* comma-delimited list of regs	*/
int  n_regs;				/* current number of reg vars	*/

char *segname[] = {
	".sect .text",
	".sect .data",
	".sect .data",
	".sect .bss"
};

con_part(sz, w)
	register int	sz;
	word		w;
{
	while (part_size % sz) part_size++;

	if (part_size == TEM_WSIZE)
		part_flush();
	if (sz == 1 || sz == 2) {
		w &= (sz == 1 ? 0xFF : 0xFFFF);
		w <<= 8 * part_size;
		part_word |= w;
	} else {
		assert(sz == 4);
		part_word = w;
	}
	part_size += sz;
}

con_mult(sz)
	word	sz;
{
	if (sz != 4)
		fatal("bad icon/ucon size");
	fprintf(codefile,".long\t%s\n",str);
}


regscore (offset, size, type, score, totype)
long offset;
{
    if (size > 4)			/* only consider register-sized	*/
	return -1;			/* variables			*/

    if (score < 2) return -1;

    /* give greater preference to locals */
    if (offset >= 0) score *= 4; else score *= 8;

    return score;
}


i_regsave()
{
    n_regs = 0;
    *regnames = '\0';
}


regsave(name, offset, size)
char *name;
long offset;
{

    if (n_regs > 0)			/* if its not the first item	*/
	strcat(regnames, ",");		/* prefix it with a comma	*/
    strcat(regnames, name);		/* add to list of names to save */

    reg[n_regs].name = name;		/* and add to array of reg vars	*/
    reg[n_regs++].offset = offset;
}


f_regsave()
{
    int i;
    long n;
    char *q;

    if (n_regs > 0)
	fprintf(codefile, "STMFD R12<,{%s}\n", regnames);

    for (i = 0; i < n_regs; i++) {	/* load their new values	*/
	n = reg[i].offset;
	q = reg[i].name;
    if (n > 0)				/* only proc parameters		*/
	    fprintf(codefile, "LDR %s,[R13,#%ld]\n", q, n);
    }
}


regreturn()
{
      if (n_regs > 0)
	fprintf(codefile, "LDMFD R12<,{%s}\n", regnames);
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

prolog(nlocals) full nlocals;
{
	fprintf(codefile, "STMFD R12<,{R13,R14}\n");
	fprintf(codefile, "MOV R13,R12\n");
	if (nlocals)
		fprintf(codefile, "SUB R12,R12,#%d\n", nlocals);
	return;
}

#ifdef NOFLOAT
con_float()  /* warning only -- AMK */
 {
 	static int been_here;
 
         if (argval != 4 && argval != 8)
 		fatal("bad fcon size");
 	fputs(".data4\t", codefile);
 	if (argval == 8)
 	        fputs("0,",codefile);
         fputs("0 !dummy float\n", codefile);
 	if ( !been_here++) {
 		fputs("Warning : dummy float-constant(s)\n", stderr);
 	}
 }
#else
#define IEEEFLOAT

con_float() {
	double f;
	double atof();
	int i;
	int j;
	double frexp();
#ifndef OWNFLOAT
	int sign = 0;
	int fraction[4] ;
#else OWNFLOAT
	float fl;
	char *p;
#endif OWNFLOAT

	if (argval!= 4 && argval!= 8)	{
		fprintf(stderr,"float constant size = %d\n",argval);
		fatal("bad fcon size");
	}
	fprintf(codefile,"!float %s sz %d\n", str, argval);
	f = atof(str);
	if (f == 0) {
		if (argval == 8) fprintf(codefile, ".data2 0, 0\n");
		fprintf(codefile, ".data2 0, 0\n");
		return;
	}
#ifdef OWNFLOAT
	if (argval == 4) {
		/* careful: avoid overflow */
		double ldexp();
		f = frexp(f, &i);
		fl = f;
		fl = frexp(fl,&j);
		if (i+j > 127) {
			/* overflow situation */
			fprintf(codefile, ".data1 0%o, 0377, 0377, 0377 ! overflow\n",
				f < 0 ? 0377 : 0177);
			return;
		}
		if (i+j < -127) {
			/* underflow situation */
			fprintf(codefile, ".data1 0%o, 0200, 0, 0 ! underflow\n",
				f < 0 ? 0200 : 0);
			return;
		}
		fl = ldexp(fl, i+j);
		p = (char *) &fl;
	}
	else {
		p = (char *) &f;
	}
	fprintf(codefile, ".data1 0%o", *p++ & 0377);
	for (i = argval-1; i; i--) {
		fprintf(codefile,",0%o", *p++ & 0377);
	}
#else OWNFLOAT
	f = frexp(f, &i);
	if (f < 0) {
		f = -f;
		sign = 1;
	}
	while (f < 0.5) {
		f += f;
		i --;
	}
	f = 2*f - 1.0;		/* hidden bit */
#ifdef IEEEFLOAT
	if (argval == 4) {
#endif IEEEFLOAT
		i = (i + 128) & 0377;
		fraction[0] = (sign << 15) | (i << 7);
		for (j = 6; j>= 0; j--) {
			f *= 2;
			if (f >= 1.0) {
				f -= 1.0;
				fraction[0] |= (1 << j);
			}
		}
#ifdef IEEEFLOAT
	}
	else {
		i = (i + 1024) & 03777;
		fraction[0] = (sign << 15) | (i << 4);
		for (j = 3; j>= 0; j--) {
			f *= 2;
			if (f >= 1.0) {
				fraction[0] |= (1 << j);
				f -= 1.0;
			}
		}
	}
#endif IEEEFLOAT
	for (i = 1; i < argval / 2; i++) {
		fraction[i] = 0;
		for (j = 15; j>= 0; j--) {
			f *= 2;
			if (f >= 1.0) {
				fraction[i] |= (1 << j);
				f -= 1.0;
			}
		}
	}
	if (f >= 0.5) {
		for (i = argval/2 - 1; i >= 0; i--) {
			for (j = 0; j < 16; j++) {
				if (fraction[i] & (1 << j)) {
					fraction[i] &= ~(1 << j);
				}
				else {
					fraction[i] |= (1 << j);
					break;
				}
			}
			if (j != 16) break;
		}
	}
	for (i = 0; i < argval/2; i++) {
		fprintf(codefile,
			i != 0 ? ", 0%o, 0%o" : ".data1 0%o, 0%o", 
			(fraction[i]>>8)&0377,
			fraction[i]&0377);
	}
#endif OWNFLOAT
	putc('\n', codefile);
}
#endif

