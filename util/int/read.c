/*
	Reading the EM object file
*/

/* $Id$ */

#include	<stdio.h>
#include	<local.h>		/* for VERSION */
#include	<em_spec.h>
#include	<as_spec.h>		/* for as_magic */

#include	"logging.h"
#include	"nofloat.h"
#include	"global.h"
#include	"log.h"
#include	"warn.h"
#include	"mem.h"
#include	"shadow.h"
#include	"read.h"
#include	"text.h"

#ifndef	NOFLOAT
extern double str2double();
#endif	/* NOFLOAT */

/************************************************************************
 *	Read object file contents.					*
 ************************************************************************
 *									*
 *	rd_open()	- open object file.				*
 *	rd_header()	- read object file header.			*
 *	rd_text()	- read program text.				*
 *	rd_gda()	- read global data area.			*
 *	rd_proctab()	- read procedure descriptors,			*
 *	rd_close()	- close object file.				*
 *									*
 ************************************************************************/

/* EM header Part 1 variables */

int FLAGS;

/* EM header Part 2 variables */

size NTEXT;
size NDATA;
long NPROC;
long ENTRY;
long NLINE;
size SZDATA;

PRIVATE FILE *load_fp;			/* Filepointer of load file */

PRIVATE ptr rd_repeat();
PRIVATE ptr rd_descr();
PRIVATE int rd_byte();
PRIVATE long rd_int();

rd_open(fname)
	char *fname;
{	/* Open loadfile */
	if ((load_fp = fopen(fname, "r")) == NULL) {
		fatal("Cannot open loadfile '%s'", fname);
	}
}

rd_header()
{
	/* Part 1 */
	if (rd_int(2L) != as_magic)
		fatal("Bad magic number in loadfile");

	FLAGS = rd_int(2L);

	if (rd_int(2L) != 0)
		fatal("Unresolved references in loadfile");

	if (rd_int(2L) != VERSION)
		fatal("Incorrect version number in loadfile");

	/* We only allow the following wordsize/pointersize combinations: */
	/*	2/2, 2/4, 4/4						  */
	/* A fatal error will be generated if other combinations occur.   */
	
	wsize = rd_int(2L);
	if (!(wsize == 2 || wsize == 4))
		fatal("Bad wordsize in loadfile");

	dwsize = 2 * wsize;		/* set double wordsize */
	wsizem1 = wsize - 1;		/* wordsize - 1 used often */
	
	psize = rd_int(2L);
	if (!(psize == 2 || psize == 4) || psize < wsize)
		fatal("Bad pointersize in loadfile");
	if (2 * psize > FRALimit)
		fatal("FRA maximum size too small");
	
	rd_int(2L);			/* Entry 7 is unused */
	rd_int(2L);			/* Entry 8 is unused */

	/* Part 2 */
	NTEXT = rd_int(psize);
	NDATA = rd_int(psize);
	NPROC = rd_int(psize);
	ENTRY = rd_int(psize);
	if (ENTRY < 0 || ENTRY >= NPROC)
		fatal("Bad entry point");
	NLINE = rd_int(psize);
	if (NLINE == 0) {
		warning(WNLINEZR);
		NLINE = I_MAXS4;
	}
	SZDATA = rd_int(psize);

	rd_int(psize);			/* entry 7 is unused */
	rd_int(psize);			/* entry 8 is unused */
}

rd_text()
{
	fread(text, 1, (int) DB, load_fp);
}

rd_gda()
{
	register int type, prev_type;
	register ptr pos, prev_pos;	/* prev_pos invalid if prev_type==0 */
	register long i;
	
	type = prev_type = 0;
	pos = prev_pos = i2p(0);
	for (i = 1; i <= NDATA; i++) {
		type = btol(rd_byte());
		LOG((" r6 rd_gda(), i = %ld, pos = %u", i, pos));
		if (type == 0) {
			/* repetition descriptor */
			register size count = rd_int(psize);
			
			LOG((" r6 rd_gda(), case 0: count = %lu", count));
			if (prev_type == 0) {
				fatal("Type 0 initialisation on type 0");
			}
			pos = rd_repeat(pos, count, prev_pos);
			prev_type = 0;
		}
		else {
			/* filling descriptor */
			register size count = btol(rd_byte());
			
			LOG((" r6 rd_gda(), case %d: count = %lu",
				type, count));
			prev_pos = pos;
			pos = rd_descr(type, count, prev_pos);
			prev_type = type;
		}
	}

	/* now protect the LIN and FIL area */
	dt_prot(i2p(0), (long)LINSIZE);
	dt_prot(i2p(4), psize);
}

rd_proctab()
{
	register long p;

	init_proctab();
	for (p = 0; p < NPROC; p++) {
		register long nloc = rd_int(psize);
		register ptr ep = i2p(rd_int(psize));

		add_proc(nloc, ep);
	}
	end_init_proctab();
}

rd_close()
{
	fclose(load_fp);
	load_fp = 0;
}

/************************************************************************
 *	Read functions for several types.				*
 ************************************************************************
 *									*
 *	rd_repeat()	- repeat the previous initialisation		*
 *	rd_descr()	- read a descriptor				*
 *	rd_byte()	- read one byte, return a int.			*
 *	rd_int(n)	- read n byte integer, return a long.		*
 *									*
 ************************************************************************/

/************************************************************************
 *		Reading a floating point number				*
 *									*
 *	A double is 8 bytes, so it can contain 4- and 8-byte (EM)	*
 *	floating point numbers. That's why a 4-byte floating point	*
 *	number is also stored in a double.				*
 ************************************************************************/

PRIVATE ptr rd_repeat(pos, count, prev_pos)
	ptr pos, prev_pos;
	size count;
{
	register size diff = pos - prev_pos;
	register size j;
	
	for (j = 0; j < count; j++) {
		register long i;

		for (i = 0; i < diff; i++) {
			data_loc(pos) = data_loc(pos - diff);
#ifdef	LOGGING
			/* copy shadow byte, including protection bit */
			dt_sh(pos) = dt_sh(pos - diff);
#endif	/* LOGGING */
			pos++;
		}
	}
	return pos;
}

PRIVATE ptr rd_descr(type, count, pos)
	int type;
	size count;
	ptr pos;
{
	register size j;
	char fl_rep[128];		/* fp number representation */
	register int fl_cnt;
		
	switch (type) {
	case 1:			/* m uninitialized words */
		j = count;
		while (j--) {
			dt_stw(pos, 0L);
			pos += wsize;
		}
		break;
	case 2:			/* m initialized bytes */
		j = count;
		while (j--) {
			dt_stn(pos++, btol(rd_byte()), 1L);
		}
		break;
	case 3:			/* m initialized wordsize integers */
		for (j = 0; j < count; j++) {
			dt_stw(pos, rd_int(wsize));
			pos += wsize;
		}
		break;
	case 4:			/* m initialized data pointers */
		for (j = 0; j < count; j++) {
			dt_stdp(pos, i2p(rd_int(psize)));
			pos += psize;
		}
		break;
	case 5:			/* m initialized instruction pointers */
		for (j = 0; j < count; j++) {
			dt_stip(pos, i2p(rd_int(psize)));
			pos += psize;
		}
		break;
	case 6:			/* initialized integer of size m */
	case 7:			/* initialized unsigned int of size m */
		if ((j = count) != 1 && j != 2 && j != 4)
			fatal("Bad integersize during initialisation");
		dt_stn(pos, rd_int(j), j);
		pos += j;
		break;
	case 8:			/* initialized float of size m */
		if ((j = count) != 4 && j != 8)
			fatal("Bad floatsize during initialisation");
		/* get fp representation */
		fl_cnt = 0;
		while (fl_rep[fl_cnt] = rd_byte()) {
			fl_cnt++;
			if (fl_cnt >= sizeof (fl_rep)) {
				fatal("Initialized float longer than %d chars",
					sizeof (fl_rep));
			}
		}
#ifndef	NOFLOAT
		/* store the float */
		dt_stf(pos, str2double(fl_rep), j);
#else	/* NOFLOAT */
		/* we cannot store the float */
		warning(WFLINIT);
#endif	/* NOFLOAT */
		pos += j;
		break;
	default:
		fatal("Unknown initializer type in global data.");
		break;
	}
	return pos;
}

PRIVATE int rd_byte()
{
	register int i;
	
	if ((i = getc(load_fp)) == EOF)
		fatal("EOF reached during initialization");
	return (i);
}

PRIVATE long rd_int(n)
	size n;
{
	register long l;
	register int i;
	
	l = btol(rd_byte());
	for (i = 1; i < n; i++) {
		l |= (btol(rd_byte()) << (i*8));
	}
	return (l);
}

