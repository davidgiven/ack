/* $Id$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* F I L E   N A M E S */

/* The names of the input files of every phase are passed as
 * arguments to the phase. First come the input file names,
 * then the output file names. We use a one-letter convention
 * to denote the type of file:
 *  p: procedure table file
 *  d: data table file
 *  l: EM text file (lines of EM instructions)
 *  b: basic block file (Control Flow Graph file)
 */

/* The input file names */

#define pname argv[1]
#define dname argv[2]
#define lname argv[3]
#define bname argv[4]

/* The output file names */

#define pname2 argv[5]
#define dname2 argv[6]
#define lname2 argv[7]
#define bname2 argv[8]

#define ARGSTART 9

extern FILE *openfile();	/* (char *name, *mode)
				 * Open a file with the given name
				 * and mode; aborts if the file
				 * cannot be opened.
				 */
