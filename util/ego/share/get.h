/*  I N P U T   R O U T I N E S */

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
extern bool	 getbblocks();		/* (FILE *f,short kind_out,
					 * short *n_out, bblock_p *g_out,
					 * line_p *l_out)
					 * Read the EM text of a single
					 * unit from the given file.
					 * This unit can be either a procedure
					 * or a umber of data declarations and
					 * messages. If it is a proc., then
					 * partition the text into
					 * basic blocks. Return the
					 * number of basic blocks in n_out.
					 */
extern call_p getcall();		/* (FILE *cf)
					 * Read a call from the call-file
					 */
extern line_p get_text();		/* (FILE *lf; proc_p *p_out)
					 * Read the EM text of one procedure.
					 * The procedure read is returned via
					 * p_out.
					 */
extern calcnt_p getcc();		/* (FILE *ccf; proc_p p)
					 * Read the call-count information
					 * of procedure p.
					 */
extern line_p get_ca_lines();		/* (FILE *lf; proc_p *p_out)
					 * Read em lines till end pseudo is met.
					 * (Used only by CA phase).
					 */
