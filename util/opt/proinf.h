/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Header$ */

struct num {
	num_p	n_next;
	unsigned n_number;
	unsigned n_jumps;
	num_p	n_repl;
	short	n_flags;
	short	n_size;		/* size of element on top at this label */
	line_p	n_line;
};

/* contents of .n_flags */
#define NUMDATA		000001
#define NUMREACH	000002
#define NUMKNOWN	000004
#define NUMMARK		000010
#define NUMSCAN		000020
#define NUMSET		000040
#define NUMCOND		000100
#define NUMFALLTHROUGH	000200

#define NNUMHASH	37
extern num_p	numlookup();

struct regs {
	reg_p	r_next;
	offset	r_par[4];
};

typedef struct proinf {
	offset	localbytes;
	line_p	lastline;
	sym_p	symbol;
	reg_p	freg;
	bool	gtoproc;
	num_p	numhash[NNUMHASH];
} proinf;

extern proinf curpro;
