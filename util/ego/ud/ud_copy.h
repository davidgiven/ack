/* $Id$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */

/* C O P Y   P R O P A G A T I O N */

extern line_p *copies;		/* table of copies; every entry points to the
				 * store-instruction.
				 */
extern short *def_to_copynr;	/* Table that maps a 'definition'-number to a
				 * 'copy' number.
				 */
extern short nrcopies;		/* number of copies in the current procedure
				 * (length of copies-table)
				 */

extern copy_analysis();		/* (proc_p p)
				 * Determine which copies procedure p has.
				 * Compute C_IN(b), for every basic block b.
				 */
extern bool is_copy();		/* (line_p def)
				 * See if the definition def is also a 'copy',
				 * i.e. an statement of the form
				 * 'A := B' (or, in EM terminology:
				 * a sequence 'Load Variable; Store Variable').
				 */
extern fold_var();		/* (line_p old,new; bblock_p b)
				 * The variable referenced by the
				 * EM instruction 'old' must be replaced
				 * by the variable referenced by 'new'.
				 */
extern bool value_retained();	/* (line_p copy; short defnr; line_p use;
				 * bblock_p b)
				 * See if the right hand side variable of the
				 * copy still has the same value at 'use'.
				 * If the copy and the use are in the same
				 * basic block (defnr = 0), search from the
				 * copy to the use, to see if the rhs variable
				 * is changed. If the copy is in another block,
				 * defnr is the definition-number of the copy.
				 * Search from the beginning of the block to
				 * the use, to see if the rhs is changed;
				 * if not, check that the copy is in C_IN(b).
				 */
