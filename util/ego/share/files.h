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

struct files
{
	/* Input files */

	const char* pname_in;
	const char* dname_in;
	const char* lname_in;
	const char* bname_in;

	/* Output files */

	const char* pname_out;
	const char* dname_out;
	const char* lname_out;
	const char* bname_out;

	/* The rest of the arguments. */

	const char** argv;
	int argc;
};

struct files* findfiles(int argc, const char** argv);

FILE *openfile(const char *name, const char *mode);
				/*
				 * Open a file with the given name
				 * and mode; aborts if the file
				 * cannot be opened.
				 */
