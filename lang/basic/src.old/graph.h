#

#ifndef NORCSID
# define RCS_GRAPH	"$Header$"
#endif

/* 
** The control graph is represented by a multi-list structure.
** The em code is stored on the em intermediate file already
** The offset and length is saved only.
** Although this makes code generation mode involved, it allows
** rather large BASIC programs to be processed.
*/
typedef struct LIST {
	int	emlabel;		/* em label used with forwards */
	int	linenr;			/* BASIC line number */
	struct LIST *nextlist;
} List;

typedef struct LINERECORD{
	int	emlabel;		/* target label */
	int	linenr;			/* BASIC line number */
	long	offset;			/* file offset in em file */
	long	codelines;		/* number of em code lines */
	List	*callers;		/* used from where ? */
	List	*gotos;			/* fanout labels */
	struct LINERECORD	*nextline, *prevline;
	int	fixed;			/* fixation of block */
} Linerecord;

Linerecord	*firstline, 
		*currline, 
		*lastline;
extern List	*forwardlabel;
