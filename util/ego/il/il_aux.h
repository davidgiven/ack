/* $Id$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */

/*  I N L I N E   S U B S T I T U T I O N
 *
 *  I L _ A U X . H
 */

extern int    tsize();		/* (int type)
				 * Determine the size of a variable of
				 * the given type.
				 */
extern line_p duplicate();	/* (line_p lnp)
				 * Make a duplicate of the given EM
				 * instruction. Pseudos may not be
				 * passed as argumnets.
				 */
extern line_p copy_expr();	/* (line_p l1)
				 * copy the expression l1.
				 * Pseudos may not be contained in
				 * the list of instructions.
				 */
extern rem_call();		/* (call_p c)
				 * Remove a call from main memory.
				 */
extern rem_graph();		/* (proc_p p)
				 * Remove the CFG and EM text of
				 * a procedure from core.
				 */
extern remcc();			/* (calcnt_p head)
				 * Remove call-count info from core.
				 */
extern call_p getcall();	/* (FILE *cf)
				 * Read a call from the call-file
				 */
extern line_p get_text();	/* (FILE *lf; proc_p *p_out)
				 * Read the EM text of one procedure.
				 * The procedure read is returned via
				 * p_out.
				 */
extern calcnt_p getcc();	/* (FILE *ccf; proc_p p)
				 * Read the call-count information
				 * of procedure p.
				 */
extern putcall();		/* (call_p call; FILE *cfile; short level)
				 * Write the call 
				 * with the given id to the given file.
				 * The level is the nesting level, used by
				 * putcall when it calls itself recurively.
				 * It should be 0 on outer levels.
				 */
extern long putcc();		/* (calcnt_p head; FILE *ccf)
				 * Write call-count information to
				 * file ccf.
				 */
