/*
	Startup routines
*/

/* $Id$ */

#include	<stdio.h>

#include	<em_abs.h>
#include	"logging.h"
#include	"global.h"
#include	"log.h"
#include	"alloc.h"
#include	"warn.h"
#include	"mem.h"
#include	"shadow.h"
#include	"trap.h"
#include	"read.h"


/****************************************************************
 *	The EM-machine is not implemented as a contiguous	*
 *	piece of memory. Instead there are a number of		*
 *	"floating" pieces of memory, each representing a	*
 *	specific part of the machine. There are separate	*
 *	allocations for:					*
 *		- stack and local area (stack),			*
 *		- heap area & global data area (data),		*
 *		- program text & procedure descriptors (text).	*
 *	The names in parenthesis are the names of the global	*
 *	variables used within our program, pointing to		*
 *	the beginning of such an area. The sizes of the global	*
 *	data area and the program text can be determined	*
 *	once and for all in the "rd_header" routine.		*
 ****************************************************************/

extern char **environ;

PRIVATE ptr storestring();
PRIVATE size alignedstrlen();

char *load_name;

init(ac, av)
	int ac;
	char **av;
{
	register char **p;
	register size env_vec_size;	/* size of environ vector */
	register size arg_vec_size;	/* size of argument vector */
	register size string_size = 0;	/* total size arg, env, strings */
	register ptr ARGB, vecp, strp;
	
	init_ofiles(1);			/* Initialize all output files */
	init_signals();

	/* Read the load file header, to obtain wsize and psize */
	load_name = av[0];
	rd_open(load_name);		/* Open object file */

	rd_header();			/* Read in load file header */

	/* Initialize wsize- and psize-dependent variables */

	init_rsb();
	i_minsw = (wsize == 2) ? I_MINS2 : I_MINS4;
	i_maxsw = (wsize == 2) ? I_MAXS2 : I_MAXS4;
	i_maxuw = (wsize == 2) ? I_MAXU2 : I_MAXU4;
	max_addr = i2p(((psize == 2) ? I_MAXU2 : I_MAXS4) / wsize * wsize) - 1;
	min_off = (psize == 2) ? (-MAX_OFF2-1) : (-MAX_OFF4-1);
	max_off = (psize == 2) ? MAX_OFF2 : MAX_OFF4;

	/* Determine nr of bytes, needed to store arguments/environment */

	env_vec_size = 0;		/* length of environ vector copy */
	for (p = environ; *p != (char *) 0; p++) {
		string_size += alignedstrlen(*p);
		env_vec_size += psize;
	}
	env_vec_size += psize;		/* terminating zero */

	arg_vec_size = 0;		/* length of argument vector copy */
	for (p = av; *p != (char *) 0; p++) {
		string_size += alignedstrlen(*p);
		arg_vec_size += psize;
	}
	arg_vec_size += psize;		/* terminating zero */

	/* One pseudo-register */
	ARGB = i2p(SZDATA);		/* command arguments base */

	/* Initialize segments */
	init_text();
	init_data(ARGB + arg_vec_size + env_vec_size + string_size);
	init_stack();
	init_FRA();
	init_AB_list();

	/* Initialize trap registers */
	TrapPI = 0;			/* set Trap Procedure Identifier */
	OnTrap = TR_ABORT;		/* there cannot be a trap handler yet*/
	IgnMask = PreIgnMask;		/* copy Ignore Mask from preset */

	/* Initialize Exit Status */
	ES_def = UNDEFINED;		/* set Exit Status illegal */

	/* Read partitions */

	rd_text();
	rd_gda();
	rd_proctab();

	rd_close();

	/* Set up the arguments and environment */

	vecp = ARGB;			/* start of environ vector copy */
	dppush(vecp);			/* push address of env pointer */
	strp = vecp + env_vec_size;	/* start of environ strings */
	for (p = environ; *p != (char *) 0; p++) {
		dt_stdp(vecp, strp);
		strp = storestring(strp, *p);
		vecp += psize;
	}
	dt_stdp(vecp, i2p(0));		/* terminating zero */

	vecp = strp;			/* start of argument vector copy */
	dppush(vecp);			/* push address of argv pointer */
	strp = vecp + arg_vec_size;	/* start of argument strings */
	for (p = av; *p != (char *) 0; p++) {
		dt_stdp(vecp, strp);
		strp = storestring(strp, *p);
		vecp += psize;
	}
	dt_stdp(vecp, i2p(0));		/* terminating zero */

	wpush((long) ac);	/* push argc */
}

PRIVATE size alignedstrlen(s)
	char *s;
{
	register size len = strlen(s) + 1;

	return (len + wsize - 1) / wsize * wsize;
}

PRIVATE ptr storestring(addr, s)
	ptr addr;
	char *s;
{
	/*	Store string, aligned to a fit multiple of wsize bytes.
		Return first address on a wordsize boundary after string.
	*/
	register size oldlen = strlen(s) + 1;
	register size newlen = ((oldlen + wsize - 1) / wsize) * wsize;
	register long i;

	LOG(("@g6 storestring(%lu, %s), oldlen = %ld, newlen = %ld",
			addr, s, oldlen, newlen));
	ch_in_data(addr, newlen);
	ch_aligned(addr, newlen);

	/* copy data of source string */
	for (i = 0; i < oldlen; i++) {
		data_loc(addr + i) = *s++;
		dt_int(addr + i);
	}
	/* pad until newlen */
	for (; i < newlen; i++) {
		data_loc(addr + i) = (char) 0;
		dt_int(addr + i);
	}
	return (addr + i);
}

#ifdef	LOGGING
dt_clear_area(from, to)
	ptr from;
	register ptr to;
{
	/* includes *from but excludes *to */
	register ptr a;

	for (a = from; a < to; a++) {
		dt_undef(a);
	}
}

st_clear_area(from, to)
	ptr from;
	register ptr to;
{
	/* includes both *from and *to (since ML+1 is unexpressible) */
	register ptr a;

	for (a = from; a >= to; a--) {
		st_undef(a);
	}
}
#endif	/* LOGGING */

