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
