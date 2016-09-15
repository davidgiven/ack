/* $Id$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/*  S H A R E D   F I L E
 *
 *  F I L E S . C 
 */

#include <stdio.h>
#include "files.h"

struct files* findfiles(int argc, const char** argv)
{
	static struct files files;

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

	files.pname_in = argv[1];
	files.dname_in = argv[2];
	files.lname_in = argv[3];
	files.bname_in = argv[4];

	/* The output file names */

	files.pname_out = argv[5];
	files.dname_out = argv[6];
	files.lname_out = argv[7];
	files.bname_out = argv[8];

	/* The rest of the arguments. */

	files.argv = argv + 8;
	files.argc = argc - 8;

	return &files;
}

FILE *openfile(char* name, char* mode)
{
	FILE *f;

	if ((f = fopen(name,mode)) == NULL) {
		error("cannot open %s",name);
	}
	return f;
}
