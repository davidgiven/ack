/* $Id$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/*  I N T E R M E D I A T E   C O D E
 *
 *  L O O K - U P   R O U T I N E S
 */

/* During Intermediate Code generation data label names ('symbols'),
 * procedure names and instruction labels (numbers) are translated
 * to resp. a data block pointer, a proc pointer and a label identifier.
 * We use three hash tables for this purpose (symhash, prochash, numhash).
 * Every name/number is hashed to an index in a specific table. A table
 * entry contains a list of structs (sym, prc, num), each one representing
 * a 'synonym'. (Synonyms are names/numbers having the same hash value).
 */


/* status passed as argument to look_up routines:
 * resp. used occurrence, defining occurrence, occurrence in
 * a MES ms_ext pseudo.
 */

#define OCCURRING	0
#define DEFINING	1
#define IMPORTING	2

#define NSYMHASH 127
#define NPROCHASH 127
#define NNUMHASH  37

extern sym_p symhash[];
extern prc_p prochash[];
extern num_p numhash[];

extern lab_id	instr_lab();		/* ( short number)
					 * Maps EM labels to sequential
					 * integers.
					 */
extern dblock_p	symlookup();		/* (char *ident, int status)
					 * Look up the data block with
					 * the given name.
					 */
extern dblock_p	getsym();		/* ( int status)
					 * Read and look up a symbol.
					 * If this is the first occurrence
					 * of it, then make it external
					 * (if status=OCCURRING) or
					 * internal (if DEFINING).
					 */
extern proc_p	getproc();		/* (int status)
					 * Same as getsym, but for procedure
					 * names.
					 */
extern proc_p	proclookup();		/* ( char *ident, int status)
					 * Find (in the hashtable) the
					 * procedure with the given name.
					 */
extern		cleaninstrlabs();	/* ( )
					 * Forget about all instruction labels.
					 */
extern		dump_procnames();	/* (prc_p hash[], int n, FILE *f)
					 * Save the names of the procedures
					 * in file f; hash is the hashtable
					 * used and n is its length.
					 */
extern		cleanprocs();		/* (prc_p hash[], int n,mask)
					 * Make the names of all procedures
					 * for which p_flags1&mask = 0 invisible
					 */
extern		cleandblocks();		/* (sym_p hash[], int n)
					 * Make the names of all data blocks
					 * for which d_flags1&mask = 0 invisible
					 */
