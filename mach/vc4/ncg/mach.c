/*
 * VideoCore IV code generator for the ACK
 * © 2013 David Given
 * This file is redistributable under the terms of the 3-clause BSD license.
 * See the file 'Copying' in the root of the distribution for the full text.
 */

#include <stdlib.h>
#include <limits.h>

int framesize;

/* Write out a constant data section. */

void con_part(int sz, word w)
{
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

void prolog(full nlocals)
{
	int ss = nlocals + 8;
	fprintf(codefile, "push fp, lr\n");
	fprintf(codefile, "mov fp, sp\n");
	if (nlocals > 0)
		fprintf(codefile, "sub sp, #%ld\n", nlocals);

	framesize = nlocals;
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

char *segname[] = {
	".sect .text",
	".sect .data",
	".sect .rom",
	".sect .bss"
};

