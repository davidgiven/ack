/* $Id$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/*  I N T E R M E D I A T E   C O D E
 *
 *  G L O B A L   C O N S T A N T S  &  V A R I A B L E S
 */


/* Data structures for Intermediate Code generation */

typedef struct sym *sym_p;
typedef struct prc *prc_p;
typedef struct num *num_p;


struct sym {
	sym_p	 sy_next;	/* link					*/
	char	 *sy_name;	/* name of the symbol			*/
	dblock_p sy_dblock;	/* pointer to dblock struct		*/
};
struct prc {
	prc_p	 pr_next;	/* link					*/
	char	 *pr_name;	/* name of the procedure		*/
	proc_p	 pr_proc;	/* pointer tto proc struct		*/
};


struct num {
	num_p	 n_next;	/* link					*/
	unsigned n_number;	/* EM repr. e.g. 120 in 'BRA *120'	*/
	lab_id	 n_labid;	/* sequential integer repr. of  IC	*/
};



/* macros used by ic_lib.c and ic_io.c: */

#define ARCHIVE	0
#define NO_ARCHIVE 1


/*
 * The next constants are close to sp_cend for fast switches
 */
#define INST	256	/* instruction:		number in tabval */
#define PSEU	257	/* pseudo:		number in tabval */
#define ILBX	258	/* label:		number in tabval */
#define DLBX	259	/* symbol:		name in string[] */
#define CSTX1	260	/* short constant:	stored in tabval */
#define CSTX2	261	/* offset:		value in tabval2 */
#define VALX1	262	/* symbol+short:	in string[] and tabval */
#define VALX2	263	/* symbol+offset:	in string[] and tabval2 */
#define ATEOF	264	/* bumped into end of file */

/* Global variables */

extern dblock_p	db;
extern dblock_p	hol0_db;		/* ABS block */
extern dblock_p		ldblock;	/* last dblock processed so  far   */
extern proc_p		lproc;		/* last proc processed so far  */
extern short		tabval;		/* used by table1, table2 and table3 */
extern offset		tabval2;
extern char		string[];
extern line_p		lastline;	/* last line read */
extern int		labelcount;	/* # labels in current procedure */
extern obj_id		lastoid;	/* last object identifier used	*/
extern proc_id		lastpid;	/* last proc   identifier used  */
extern lab_id		lastlid;	/* last label  identifier used	*/
extern dblock_id	lastdid;	/* last dblock identifier used	*/

extern byte em_flag[];

