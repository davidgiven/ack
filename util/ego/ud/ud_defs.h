/* $Id$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/*  U S E  -  D E F I N I T I O N   A N A L Y S I S
 *
 *  U D _ D E F S . H
 */

extern short nrdefs;		/* total number of definitions */
extern short nrexpldefs;	/* number of explicit definitions */
extern line_p *defs;		/* map of explicit definitions */
extern cset *vardefs;		/* set of explicit defs. of all variables */

void make_defs(proc_p p);	/*
				 * Compute defs[], vardefs[]
				 * and CHGVARS(b) (for every b).
				 */
void gen_sets(proc_p p);	/*
				 * Compute GEN(b) (for every b).
				 */
void kill_sets(proc_p p);	/*
				 * Compute KILL(b) (for every b).
				 */
bool does_expl_def(line_p l);	/*
				 * See if instruction l does an explicit
				 * definition (e.g. a STL).
				 */
bool does_impl_def(line_p l);	/*
				 * See if instruction l does an implicit
				 * definition (e.g. a CAL).
				 */


/* Two kinds of definitions exist:
 *  - an explicit definition is an assignment to a single
 *    variable (e.g. a STL, STE, INE).
 *  - an implicit definition is an assignment to a variable
 *    performed via a subroutine call or an
 *    indirect assignment (through a pointer).
 * Every explicit definition has an 'explicit definition number',
 * which is its index in the 'defs' table.
 * Every implicit definition has an 'implicit definition number',
 * which is the 'variable number' of the changed variable.
 * Every such definition also has a 'definition number'.
 * Conversions exist between these numbers.
 */

#define TO_EXPLICIT(defnr)	(defnr - nrvars)
#define TO_IMPLICIT(defnr)	(defnr)
#define EXPL_TO_DEFNR(explnr)	(explnr + nrvars)
#define IMPL_TO_DEFNR(implnr)	(implnr)
#define IMPLICIT_DEF(v)		(v)
#define IMPL_VAR(defnr)		(defnr)
#define IS_IMPL_DEF(defnr)	(defnr <= nrvars)
