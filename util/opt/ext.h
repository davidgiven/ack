/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Id$ */

#ifndef FILE
#include <stdio.h>
#endif
extern	unsigned linecount;
extern	int	prodepth;
extern	bool	Lflag;
extern	bool	nflag;
extern	int	repl_muls;
extern	bool	repl_longmuls;
extern	byte	em_flag[];
extern	line_p	instrs,pseudos;
extern	FILE	*outfile;
extern	char	tempname[];
extern	offset	wordsize;
extern	offset	pointersize;
extern	char	*progname;
