/* $Id$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
 /*  O U T P U T   R O U T I N E S */


extern FILE *curoutp;	/* current output file */

#define outbyte(b) putc(b,curoutp)
extern	outshort();		/* (short i)
				 * Write a short to curoutp
				 */
extern outoff();		/* (offset off)
				 * Write an offset to curoutp
				 */
extern	outproc();		/* (proc_p p)
				 * Write a procid to curoutp
				 */
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
