/* $Id$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/*  I N P U T   R O U T I N E S */

extern	FILE	*curinp;		/* current input file */
extern block_id lastbid;		/* block identifying number */
extern lab_id	 lastlabid;		/* last label identifier */

#define getbyte()	getc(curinp)
extern short	getshort();		/* ()
					 * Read a short from curinp
					 */
extern offset	getoff();		/* ()
					 * Read an offset from curinp
					 */
extern line_p read_line();		/* ( proc_p *p_out)
					 * Read a line of EM code (i.e. one 
					 * instruction)  and its arguments 
					 * (if any). If the instruction is a
					 * 'pro' pseudo, set p_out.
					 */

extern line_p getlines();		/* ( FILE *lf; int n; proc_p *p_out;
					 *   bool collect_mes)
					 * Read n lines of EM text and doubly
					 * link them.  Also process messages
					 * if required.
					 */

extern bblock_p freshblock();		/* ()
					 * Allocate a bblock struct and assign
					 * it a brand new block_id.
					 */
extern lab_id freshlabel();		/* ()
					 * Get a brand new lab_id.
					 */
extern dblock_p getdtable();		/* (char *dname)
					 * Read the data block table from
					 * the file with the given name.
					 */
extern proc_p	getptable();		/* (char *pname)
					 * Read the proc table from
					 * the file with the given name.
					 */
extern bool	 getunit();		/* (FILE *gf,*lf; short kind_out;
					 * bblock_p g_out; line_p l_out;
					 * proc_p *p_out; bool collect_mes)
					 * Read the control flow graph
					 * (from file gf) and the EM text
					 * (from lf). If collect_mes is TRUE,
					 * all register messages will be
					 * collected and put in the global
					 * variable 'mesregs'. The proc read
					 * is returned in p_out.
					 */
extern		message();		/* (line_p lnp)
					 * See if  lnp is some useful message.
	 				 * (e.g. a message telling that a
					 * certain local variable will never be
					 * referenced indirectly, so it may be
					 * put in a register.
					 * If so, add it to the mesregs set.)
	 				 */
