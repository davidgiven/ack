 /*  O U T P U T   R O U T I N E S */



extern	putdtable();		/* (dblock_p head, FILE *df)
				 * Write the data block table to file df,
				 * preceded by its length.
				 */
extern	putptable();		/* (proc_p head, FILE *pf, bool all)
				 * Write the proc table to file pf,
				 * preceded by its length. If all=false,
				 * the fields computed by CF will not be
				 * written (used by the IC phase).
				 */
extern	putunit();		/* (short kind; proc_p p; line_p l;
				 * FILE *gf, *lf)
				 * If kind = LTEXT, then write
				 * the control flow graph to file gf,
				 * preceded by its length (#basic blocks);
				 * write the EM code of every basic block
				 * in the graph to file lf, preceded by
				 * the number of instructions in the block.
				 * Else, (kind = LDATA) just write the
				 * list of instructions (data declarations)
				 * to lf.
				 */
extern short putlines();	/* (line_p l; FILE *lf)
				 * Output the list of em instructions
				 * headed by l. Return the number of
				 * instructions written.
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
