/* $Id$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/*  I N L I N E   S U B S T I T U T I O N
 *
 *  I L 3 _ C H A N G E . C
 */


extern chg_callseq();		/* (call_p c; line_p cal, *l_out)
				 * Change the calling sequence of
				 * the call c. The parameters are
				 * changed and the sequence
				 * CAL - ASP - LFR is removed.
				 * cal points to the CAL instruction
				 * l_out indicates where the expanded
				 * text of the called routine must
				 * be put.
				 */
extern line_p make_label();	/* (line_p l; proc_p p)
				 * Make sure that the instruction after
				 * l contains a label. If this is not
				 * already the case, create a new label.
				 */
extern modify();		/* (line_p text; call_p c; line_p lab;
				 *  offset ab_off, lb_off; int lab_off)
				 * Modify the EM text of the called
				 * procedure.
				 */
extern mod_actuals();		/* (call_p nc,c; line_p lab;
				 *  offset ab_off, lb_off; int lab_off)
				 * Modify the actual parameters of the
				 * call nc the same way as the text of
				 * call c would be modified.
				 */
extern insert();		/* (line_p text,l,firstline)
				 * Insert the modified EM text.
				 * Pseudos are put after the pseudos
				 * of the caller.
				 */
extern liquidate();		/* (proc_p p; line_p text)
				 * All calls to p were expanded in line,
				 * so p is no longer needed.
				 */
