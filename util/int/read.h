/*
	Load-time variables, for reading the EM object file
*/

/* $Id$ */

/* EM header Part 1 varaibles */

extern int FLAGS;

/* EM header Part 2 variables */

extern size NTEXT;		/* number of programtext bytes */
extern size NDATA;		/* number of load-file descriptor bytes */
extern long NPROC;		/* number of procedure descriptors */
extern long ENTRY;		/* procedure identifier of start procedure */
extern long NLINE;		/* the maximum source line number */
extern size SZDATA;		/* number of gda bytes after initialization */

/* Open e.out file with "fname". Raise a fatal error if
   it cannot be opened. */
void rd_open(char *fname);

/* Read the header of the load file and populates
 * the "FLAGS", "NTEXT", "NDATA", "NPROC", "ENTRY", "NLINE"
 * and "SZDATA" variables. A fatail error is raised
 * if there is an error reading the load file.
 */
void rd_header(void);

/* Read the text segment from the load file into the
 * the address pointed to by the "text" variable.
 */
void rd_text(void);

/* Read and populate the data segment from the load file
 * into the address pointed to by the "data" variable.
 */
void rd_gda(void);

/* Read the procedure table from the load file. */
void rd_proctab(void);

/* Close the load file. */
void rd_close(void);
