/* $Id$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */

/* 
 *  L O C A L S . H
 */

extern local_p *locals;		/* table of locals, index is local-number */
extern short   nrlocals;	/* number of locals for which we keep ud-info */

extern make_localtab();		/* (proc_p p) 
				 * Analyse the text of procedure p to determine
				 * which local variable p has. Make a table of
				 * these variables ('locals') and count them
				 * ('nrlocals'). Also collect register messages.
				 */
extern var_nr();		/* (line_p l; short *nr_out;bool *found_out)
				 * Compute the 'variable number' of the
				 * variable referenced by EM instruction l.
				 */
extern find_local();		/* (offset off; short *nr_out; bool *found_out)
				 * Try to find the local variable at the given
	 			 * offset. Return its local-number.
	 			 */


/* Every global variable for which ud-info is maintained has
 * a 'global variable number' (o_globnr). Every useful local
 * has a 'local variable number', which is its index in the
 * 'locals' table. All these variables also have a
 * 'variable number'. Conversions exist between these numbers.
 */

#define TO_GLOBAL(v)	(v)
#define TO_LOCAL(v)	(v - nrglobals)
#define GLOB_TO_VARNR(v) (v)
#define LOC_TO_VARNR(v) (v + nrglobals)
#define IS_GLOBAL(v)	(v <= nrglobals)
#define IS_LOCAL(v)	(v > nrglobals)

#define REGVAR(lc)	lc->lc_flags |= LCF_REG
#define IS_REGVAR(lc)	(lc->lc_flags & LCF_REG)
#define BADLC(lc)	lc->lc_flags |= LCF_BAD
#define IS_BADLC(lc)	(lc->lc_flags & LCF_BAD)


