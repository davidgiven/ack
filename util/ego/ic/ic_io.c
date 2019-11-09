/* $Id$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/*  I N T E R M E D I A T E   C O D E
 *
 *  I C _ I O . C
 */



#include <stdio.h>
#include <em_pseu.h>
#include <em_spec.h>
#include <arch.h>
#include "../share/types.h"
#include "../share/debug.h"
#include "ic.h"
#include "ic_lookup.h"
#include "../share/alloc.h"
#include "ic_io.h"


STATIC short libstate;
STATIC long  bytecnt;

STATIC FILE *infile;  /* The current EM input file */

STATIC int readbyte()
{
	if (libstate == ARCHIVE && bytecnt-- == 0L) {
		/* If we're reading from an archive file, we'll
		 * have to count the number of characters read,
		 * to know where the current module ends.
		 */
		return EOF;
	}
	return getc(infile);
}




short readshort() {
	register int l_byte, h_byte;

	l_byte = readbyte();
	h_byte = readbyte();
	if ( h_byte>=128 ) h_byte -= 256 ;
	return l_byte | (h_byte*256) ;
}

#ifdef LONGOFF
offset readoffset() {
	register long l;
	register int h_byte;

	l = readbyte();
	l |= ((unsigned) readbyte())*256 ;
	l |= readbyte()*256L*256L ;
	h_byte = readbyte() ;
	if ( h_byte>=128 ) h_byte -= 256 ;
	return l | (h_byte*256L*256*256L) ;
}
#endif


short get_int() {

	switch(table2()) {
	default: error("int expected");
	case CSTX1:
		return(tabval);
	}
}

char readchar()
{
	return(readbyte());
}



offset get_off() {

	switch (table2()) {
	default: error("offset expected");
	case CSTX1:
		return((offset) tabval);
#ifdef LONGOFF
	case CSTX2:
		return(tabval2);
#endif
	}
}

STATIC void make_string(n) int n; {
	
	sprintf(string,".%u",n);
}

STATIC void inident() {
	register int n;
	register char *p = string;
	register int c;

	n = get_int();
	while (n--) {
		c = readbyte();
		if (p<&string[IDL])
			*p++ = c;
	}
	*p++ = 0;
}

int table3(n) int n; {

	switch (n) {
	case sp_ilb1:	tabval = readbyte(); return(ILBX);
	case sp_ilb2:	tabval = readshort(); return(ILBX);
	case sp_dlb1:	make_string(readbyte()); return(DLBX);
	case sp_dlb2:	make_string(readshort()); return(DLBX);
	case sp_dnam:	inident(); return(DLBX);
	case sp_pnam:	inident(); return(n);
	case sp_cst2:	tabval = readshort(); return(CSTX1);
#ifdef LONGOFF
	case sp_cst4:	tabval2 = readoffset(); return(CSTX2);
#endif
	case sp_doff:	if (table2()!=DLBX) error("symbol expected");
			switch(table2()) {
			default:	error("offset expected");
			case CSTX1:		return(VALX1);
#ifdef LONGOFF
			case CSTX2:		return(VALX2);
#endif
			}
	default:	return(n);
	}
}

int table1() {
	register int n;

	n = readbyte();
	if (n == EOF)
		return(ATEOF);
	if ((n <= sp_lmnem) && (n >= sp_fmnem)) {
		tabval = n;
		return(INST);
	}
	if ((n <= sp_lpseu) && (n >= sp_fpseu)) {
		tabval = n;
		return(PSEU);
	}
	if ((n < sp_filb0 + sp_nilb0) && (n >= sp_filb0)) {
		tabval = n - sp_filb0;
		return(ILBX);
	}
	return(table3(n));
}

int table2() {
	register int n;

	n = readbyte();
	if ((n < sp_fcst0 + sp_ncst0) && (n >= sp_fcst0)) {
		tabval = n - sp_zcst0;
		return(CSTX1);
	}
	return(table3(n));
}




void file_init(FILE *f, short state, long length)
{
	short n;

	infile = f;
	libstate = state;
	bytecnt = length;
	linecount = 0;
	n = readshort();
	if (n != (short) sp_magic) {
		error("wrong magic number: %d", n);
	}
}



void arch_init(arch)
	FILE *arch;
{
	short n;

	infile = arch;
	n = readshort();
	if (n != (short)ARMAG && n != (short)AALMAG) {
		error("wrong archive magic number: %d",n);
	}
}
