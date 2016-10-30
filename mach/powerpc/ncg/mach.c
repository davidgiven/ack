/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 *
 */

#include <stdlib.h>
#include <limits.h>

#ifndef NORCSID
static char rcsid[]=	"$Id$" ;
#endif

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
	fprintf(codefile, "addi sp, sp, %d\n", -ss);
	fprintf(codefile, "stw fp, %d(sp)\n", nlocals);
	fprintf(codefile, "mfspr r0, lr\n"
	                  "stw r0, %d(sp)\n", nlocals+4);
	fprintf(codefile, "addi fp, sp, %d\n", nlocals);
	
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

i_regsave()
{
	int i;
	
	fprintf(codefile, "! i_regsave()\n");
	for (i=0; i<32; i++)
		savedregsi[i] = INT_MAX;
	numsaved = 0;
}

/* Mark a register as being saved. */

regsave(const char* regname, full offset, int size)
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

void saveloadregs(const char* ops, const char* opm)
{
	int offset = -(framesize + numsaved*4);
	int reg = 32;
	
	/* Check for the possibility of a multiple. */
	
	do
	{
		reg--;
	}
	while ((reg > 0) && (savedregsi[reg] != INT_MAX));
	if (reg < 31)
	{
		fprintf(codefile, "%s r%d, %d(fp)\n", opm, reg+1, offset);
		offset += (31-reg)*4;
	}
	
	/* Saved everything else singly. */
	
	while (reg > 0)
	{
		if (savedregsi[reg] != INT_MAX)
		{
			fprintf(codefile, "%s r%d, %d(fp)\n", ops, reg, offset);
			offset += 4;
		}
		reg--;
	}
}

f_regsave()
{
	int i;
	fprintf(codefile, "! f_regsave()\n");
	fprintf(codefile, "addi sp, sp, %d\n", -numsaved*4);
	
	saveloadregs("stw", "stmw");
	
	for (i=0; i<32; i++)
		if ((savedregsi[i] != INT_MAX) && (savedregsi[i] > 0))
			fprintf(codefile, "lwz r%d, %d(fp)\n", i, savedregsi[i]);
}

/* Restore all saved registers. */

regreturn()
{
	fprintf(codefile, "! regreturn()\n");
	saveloadregs("lwz", "lmw");
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
