/*
 * VideoCore IV code generator for the ACK
 * © 2013 David Given
 * This file is redistributable under the terms of the 3-clause BSD license.
 * See the file 'Copying' in the root of the distribution for the full text.
 */

#include <stdlib.h>
#include <limits.h>

int framesize;

/*
 * machine dependent back end routines for the Zilog Z80.
 */

con_part(int sz, word w)
{
	while (part_size % sz)
		part_size++;
	if (part_size == TEM_WSIZE)
		part_flush();
	if (sz == 1) {
		w &= 0xFF;
		w <<= 8*(3-part_size);
		part_word |= w;
	} else if (sz == 2) {
		w &= 0xFFFF;
		if (part_size == 0) {
			/* Shift 8 for m68k2, 16 otherwise */
			w <<= 4 * TEM_WSIZE;
		}
		part_word |= w;
	} else {
		assert(sz == TEM_WSIZE);
		part_word = w;
	}
	part_size += sz;
}

con_mult(word sz)
{

	if (argval != 4)
		fatal("bad icon/ucon size");
	fprintf(codefile,".data4 %s\n", str);
}

#define CODE_GENERATOR  
#define IEEEFLOAT  
#define FL_MSL_AT_LOW_ADDRESS	1
#define FL_MSW_AT_LOW_ADDRESS	1
#define FL_MSB_AT_LOW_ADDRESS	1
#include <con_float>

void prolog(full nlocals)
{
	int ss = nlocals + 8;
	fprintf(codefile, "push fp, lr\n");
	fprintf(codefile, "mov fp, sp\n");
	if (nlocals > 0)
		fprintf(codefile, "sub sp, #%d\n", nlocals);

	framesize = nlocals;
}

mes(word type)
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

#ifdef REGVARS

static int savedregsi[32];
static int numsaved;

/* Initialise regvar system for one function. */

void i_regsave(void)
{
	int i;
	
	fprintf(codefile, "! i_regsave()\n");
	for (i=0; i<32; i++)
		savedregsi[i] = INT_MAX;
	numsaved = 0;
}

/* Mark a register as being saved. */

void regsave(const char* regname, full offset, int size)
{
	int regnum = atoi(regname+1);
	savedregsi[regnum] = offset;
	numsaved++;
	
	fprintf(codefile, "! %d is saved in %s\n", offset, regname);
#if 0
	fprintf(codefile, "stwu %s, -4(sp)\n", regname);
	if (offset >= 0)
		fprintf(codefile, "lwz %s, %d(fp)\n", regname, offset);
#endif
}

/* Finish saving ragisters. */

static void saveloadregs(const char* op)
{
	int minreg = 32;
	int maxreg = -1;
	int i;

	for (i=0; i<32; i++)
	{
		if (savedregsi[i] != INT_MAX)
		{
			if (i < minreg)
				minreg = i;
			if (i > maxreg)
				maxreg = i;
		}
	}

	if (minreg != 32)
	{
		fprintf(codefile, "! saving registers %d to %d\n", minreg, maxreg);
		assert(minreg == 6);

		fprintf(codefile, "%s r6-r%d\n", op, maxreg);
	}
}

f_regsave()
{
	int i;
	fprintf(codefile, "! f_regsave()\n");
	saveloadregs("push");

	for (i=0; i<32; i++)
	{
		int o = savedregsi[i];
		if ((o != INT_MAX) && (o > 0))
			fprintf(codefile, "ld r%d, %d (fp)\n", i, savedregsi[i]);
	}
}

/* Restore all saved registers. */

regreturn()
{
	fprintf(codefile, "! regreturn()\n");
	saveloadregs("pop");
}

/* Calculate the score of a given register. */

int regscore(full offset, int size, int type, int frequency, int totype)
{
	int score;
	
	fprintf(codefile, "! regscore(%ld, %d, %d, %d, %d)\n", offset, size, type, frequency, totype);
	
	if (size != 4)
		return -1;
	
	/* Per use: 6 bytes (on average)
	 * Overhead in prologue: 4 bytes, plus 4 if a parameter
	 * Overhead in epilogue: 0 bytes
	 */
	 
	score = frequency*6 - 4 - ((offset>=0) ? 4 : 0);
	fprintf(codefile, "! local at offset %d has regvar score %d\n", offset, score);
	return score;
}

#endif
