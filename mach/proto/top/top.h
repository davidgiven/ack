/* $Id$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* Tunable constants; may be overruled by machine descriptor table */

#ifndef OP_SEPARATOR
#define OP_SEPARATOR ','
#endif
#ifndef LABEL_TERMINATOR
#define LABEL_TERMINATOR	':'
#endif
#ifndef LABEL_STARTER
#define LABEL_STARTER	'I'
#endif
#ifndef OPC_TERMINATOR
#define OPC_TERMINATOR	' '
#endif

#ifndef MAX_OPC_LEN
#define MAX_OPC_LEN	10
#endif
#ifndef MAXOPLEN
#define MAXOPLEN	25
#endif
#ifndef MAXOP
#define MAXOP		2
#endif
#ifndef MAXLINELEN
#define MAXLINELEN	100
#endif
#ifndef MAXVARLEN
#define MAXVARLEN	25
#endif


typedef struct instruction *instr_p;
typedef struct pattern_descr *patdescr_p;
typedef struct instr_descr *idescr_p;
typedef struct templat *templ_p;

struct instruction {
	instr_p	fw;
	instr_p	bw;
	char	line[MAXLINELEN+1];
	char	*rest_line;
	char	opc[MAX_OPC_LEN+1];
	char	op[MAXOP][MAXOPLEN+1];
	short	state;
};

/* state: */
#define	ONLY_OPC	0
#define JUNK		1
#define	DONE		2

struct variable {
	int	vstate;
	char	value[MAXVARLEN];
};

/* vstate: */
#define UNINSTANTIATED	0
#define INSTANTIATED	1

struct pattern_descr {
	int	patlen;
	idescr_p pat;
	int	replen;
	idescr_p repl;
};

struct templat {
	char	*lctxt;
	int	varno;
	char	*rctxt;
};

struct instr_descr {
	char		*opcode;
	struct templat templates[MAXOP];
};

typedef int bool;

#define TRUE 	1
#define FALSE	0

#define NIL (instr_p) 0
#define NULLSTRING (char *) 0
