
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
