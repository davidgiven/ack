/* $Id$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* M A P . H */

extern short plength;	/* length of pmap, i.e. number of procs */
extern short olength;	/* length of omap, i.e. number of objects */
extern short llength;	/* length of lmap and lbmap, i.e.
			 * # instruction labels in current proc.
			 */
extern short lplength;	/* length of lpmap, i.e. number of loops
			 * in current procedure.
			 */
extern short blength;	/* length of bmap, i.e. number of basic blocks
			 * in current procedure.
			 */


extern line_p *lmap;	/* contains for every label_id its
			 * defining occurrence (line structure)
			 *   label_id --> line_p
			 */
extern bblock_p *lbmap; /* contains for every label_id its
			 * basic block.
			 *   label_id --> bblock_p
			 */
extern proc_p	*pmap;	/* contains for every proc_id its proc structure
			 *   proc_id --> proc_p
			 */
extern obj_p	*omap;	/* contains for every obj_id its object struct
			 *   obj_id --> obj_p
			 */
extern loop_p	*lpmap;	/* contains for every loop_id its loop struct
			 *   loop_id --> loop_p
			 */
extern bblock_p	*bmap;	/* contains for every block_id its bblock struct
			 *   block_id --> bblock_p
			 */

extern dblock_p	fdblock;/* first dblock, heads dblock list */
extern proc_p	fproc;	/* first proc, heads proc table */
