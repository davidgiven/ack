/* $Id$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/*  I N T E R M E D I A T E   C O D E
 *
 *  A U X I L I A R Y   R O U T I N E S
 */



offset opr_size(short instr);		/*
					 * size of operand of given instruction.
					 * The operand is an object , so the
					 * instruction can be loe, zre etc..
					 */
void dblockdef(dblock_p db, int n, line_p lnp);
					/*
					 * Fill in d_pseudo, d_size and
					 * d_values fields of db.
					 */
void combine(dblock_p db, line_p l1, line_p l2, byte pseu);
					/*
					 * Combine two successive ROMs or CONs
					 * (with no data label in between)
					 * into one ROM or CON.
					 */
line_p arglist(int m);			/*
					 * Read a list of m arguments. If m
					 * is 0, then the list is of
					 * undetermined length; it is
					 * then terminated by a cend symbol.
					 */
bool is_datalabel(line_p l);		/*
					 * TRUE if l is a data label defining
					 * occurrence (i.e. its l_instr
					 * field is ps_sym).
					 */
dblock_p block_of_lab(char *ident);	/*
					 * Find the datablock with
					 * the given name.
					 */
obj_p object(char *ident, offset off, offset size);
					/*
					 * Create an object struct.
					 */
