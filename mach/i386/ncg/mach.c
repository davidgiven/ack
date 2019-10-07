/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 *
 */

#ifndef NORCSID
static char rcs_m[]= "$Id$" ;
static char rcs_mh[]= ID_MH ;
#endif

#include <stb.h>

/*
 * machine dependent back end routines for the Intel 80386
 */

void
con_part(sz,w) register sz; word w; {

	while (part_size % sz)
		part_size++;
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

void
con_mult(word sz) {

	if (sz != 8)
		fatal("bad icon/ucon size");
	fprintf(codefile,".data8\t%s\n", str);
}

#define CODE_GENERATOR  
#define IEEEFLOAT  
#define FL_MSL_AT_LOW_ADDRESS	0
#define FL_MSW_AT_LOW_ADDRESS	0
#define FL_MSB_AT_LOW_ADDRESS	0
#include <con_float>

/*

string holstr(n) word n; {

	sprintf(str,hol_off,n,holno);
	return(mystrcpy(str));
}
*/

#ifdef REGVARS
full lbytes;
#endif

void
prolog(nlocals) full nlocals; {

	fputs("push ebp\nmov ebp,esp\n", codefile);
#ifdef REGVARS
	lbytes = nlocals;
#else
#ifdef NOTDEF
	probably not better on 386.
	switch(nlocals) {
	case 8:
		fputs("push eax\n", codefile);
		/* fall through */
	case 4:
		fputs("push eax\n", codefile);
		break;
	default:
#endif
		fprintf(codefile, "sub\tesp,%ld\n",nlocals);
#ifdef NOTDEF
		break;
	}
#endif
#endif
}

#ifdef REGVARS
long si_off;
long di_off;
int firstreg;

int
regscore(off, size, typ, score, totyp)
	long off;
{
	if (size != 4) return -1;
	score -= 1;
	score += score;
	if (typ == reg_pointer || typ == reg_loop) score += (score >> 2);
	score -= 2;	/* cost of saving */
	if (off >= 0) score -= 3;
	return score;
}

void
i_regsave()
{
	si_off = -1;
	di_off = -1;
	firstreg = 0;
}

void
f_regsave()
{
	if (si_off != di_off) {
		if (si_off == -lbytes) lbytes -= 4;
		if (di_off == -lbytes) lbytes -= 4;
		if (si_off == -lbytes) lbytes -= 4;
	}
	if (lbytes) fprintf(codefile, "sub\tesp,%ld\n",(long) lbytes);
	if (firstreg == 1) {
		fputs("push edi\n", codefile);
		if (si_off != -1) fputs("push esi\n", codefile);
	}
	else if (firstreg == -1) {
		fputs("push esi\n", codefile);
		if (di_off != -1) fputs("push edi\n", codefile);
	}
	if (si_off >= 0)
		fprintf(codefile, "mov esi,%ld(ebp)\n", si_off);
	if (di_off >= 0)
		fprintf(codefile, "mov edi,%ld(ebp)\n", di_off);
}

void
regsave(regstr, off, size)
	const char *regstr;
	long off;
{
	if (strcmp(regstr, "esi") == 0) {
		if (! firstreg) firstreg = -1;
		si_off = off;
	}
	else {
		if (! firstreg) firstreg = 1;
		di_off = off;
	}
}

void
regreturn()
{
	if (firstreg == 1) {
		if (si_off != -1) fputs("pop esi\n", codefile);
		fputs("pop edi\n", codefile);
	}
	else if (firstreg == -1) {
		if (di_off != -1) fputs("pop edi\n", codefile);
		fputs("pop esi\n", codefile);
	}
	fputs("leave\nret\n", codefile);
}
#endif /* REGVARS */

#ifdef MACH_OPTIONS
static int gdb_flag = 0;
static char *fp_hook_nam;

void
mach_option(s)
	char *s;
{
	if (! strcmp(s, "-gdb")) {
		gdb_flag = 1;
	}
	else if (s[1] == 'F') {
		fp_hook_nam = &s[2];
	}
	else {
		error("Unknown flag %s", s);
	}
}
#endif /* MACH_OPTIONS */

void
mes(type) word type ; {
	int argt, a1, a2 ;

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
	case ms_stb:
		argt = getarg(str_ptyp | cst_ptyp);
		if (argt == sp_cstx)
			fputs(".symb \"\", ", codefile);
		else {
			fprintf(codefile, ".symb \"%s\", ", str);
			argt = getarg(cst_ptyp);
		}
		a1 = argval;
		argt = getarg(cst_ptyp);
		a2 = argval;
		argt = getarg(cst_ptyp|nof_ptyp|sof_ptyp|ilb_ptyp|pro_ptyp);
#ifdef MACH_OPTIONS
		if (gdb_flag) {
			if (a1 == N_PSYM) {
				/* Change offset from AB into offset from
				   the frame pointer (bp).
				*/
				argval += 8;
			}
		}
#endif
		fprintf(codefile, "%s, 0x%x, %d\n", strarg(argt), a1, a2);
		argt = getarg(end_ptyp);
		break;
	case ms_std:
		argt = getarg(str_ptyp | cst_ptyp);
		if (argt == sp_cstx)
			str[0] = '\0';
		else {
			argt = getarg(cst_ptyp);
		}
		swtxt();
		if (argval == N_SLINE
#ifdef MACH_OPTIONS
		    && ! gdb_flag
#endif
		) {
			fputs("call ___u_LiB\n", codefile);
			cleanregs();	/* debugger might change variables */
		}
		fprintf(codefile, ".symd \"%s\", 0x%x,", str, (int) argval);
		argt = getarg(cst_ptyp);
		fprintf(codefile, "%d\n", (int) argval);
		argt = getarg(end_ptyp);
		break;
#ifdef MACH_OPTIONS
	case ms_flt:
		if (fp_hook_nam) {
			part_flush();
			ex_ap(fp_hook_nam);
		}
		/* fall through */
#endif
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
