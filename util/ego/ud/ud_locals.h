/*  U S E  -  D E F I N I T I O N   A N A L Y S I S
 *
 *  U D _ L O C A L S . H
 */

extern local_p *locals;		/* table of locals, index is local-number */
extern short   nrlocals;	/* number of locals for which we keep ud-info */

extern make_localtab();		/* (proc_p p) 
				 * Analyse the text of procedure p to determine
				 * which local variable p has. Make a table of
				 * these variables ('locals') and count them
				 * ('nrlocals'). Also collect register messages.
				 */
extern var_nr();		/* (line_p l; short *nr_out;bool *found_out)
				 * Compute the 'variable number' of the
				 * variable referenced by EM instruction l.
				 */
