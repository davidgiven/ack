/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
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

#ifdef NOFLOAT
con_float() {

static int been_here;
	if (argval != 4 && argval != 8)
		fatal("bad fcon size");
	fputs(".data4\t", codefile);
	if (argval == 8)
		fputs("0,", codefile);
	fputs("0 !dummy float\n", codefile);
	if ( !been_here++)
	{
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

/*

string holstr(n) word n; {

	sprintf(str,hol_off,n,holno);
	return(mystrcpy(str));
}
*/

prolog(nlocals) full nlocals; {

	fputs("\tpush\tbp\n\tmov\tbp,sp\n", codefile);
	switch (nlocals) {
	case 4: fputs("\tpush\tax\n", codefile);
	case 2: fputs("\tpush\tax\n", codefile);
	case 0: break;
	default:
		fprintf(codefile, "\tsub\tsp,%d\n",nlocals); break;
	}
}

#ifdef REGVARS
long si_off;
long di_off;
int firstreg;

regscore(off, size, typ, score, totyp)
	long off;
{
	if (size != 2) return -1;
	score -= 1;
	score += score;
	if (typ == reg_pointer || typ == reg_loop) score += (score >> 2);
	score -= 2;	/* cost of saving */
	if (off >= 0) score -= 3;
	return score;
}

i_regsave()
{
	si_off = -1;
	di_off = -1;
	firstreg = 0;
}

f_regsave()
{
}

regsave(regstr, off, size)
	char *regstr;
	long off;
{
	if (strcmp(regstr, "si") == 0) {
		if (! firstreg) firstreg = -1;
		si_off = off;
		fputs("push si\n", codefile);
		if (off >= 0)
			fprintf(codefile, "mov si,%ld(bp)\n", off);
	}
	else {
		if (! firstreg) firstreg = 1;
		di_off = off;
		fputs("push di\n", codefile);
		if (off >= 0)
			fprintf(codefile, "mov di,%ld(bp)\n", off);
	}
}

regreturn()
{
	if (firstreg == 1) {
		if (si_off != -1) fputs("jmp .sdret\n", codefile);
		else fputs("jmp .dret\n", codefile);
	}
	else if (firstreg == -1) {
		if (di_off != -1) fputs("jmp .dsret\n", codefile);
		else fputs("jmp .sret\n", codefile);
	}
	else fputs("jmp .cret\n", codefile);
}
#endif REGVARS

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
				fprintf(codefile, ".define %s\n",argstr) ;
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
