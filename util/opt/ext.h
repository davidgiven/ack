/* $Header$ */

#ifndef FILE
#include <stdio.h>
#endif
extern	unsigned linecount;
extern	int	prodepth;
extern	bool	Lflag;
extern	bool	nflag;
extern	byte	em_flag[];
extern	line_p	instrs,pseudos;
extern	FILE	*outfile;
extern	char	template[];
extern	offset	wordsize;
extern	offset	pointersize;
extern	char	*progname;
