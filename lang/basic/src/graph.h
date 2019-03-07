/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */

#ifndef  __GRAPH_H_INCLUDED__
#define  __GRAPH_H_INCLUDED__

#ifndef NORCSID
# define RCS_GRAPH	"$Id$"
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
	List	*callers;		/* used from where ? */
	List	*gotos;			/* fanout labels */
	struct LINERECORD	*nextline, *prevline;
	int	fixed;			/* fixation of block */
} Linerecord;

extern Linerecord	*firstline, 
		*currline, 
		*lastline;
extern List	*forwardlabel;

extern List	*gosublabel();
extern void jumpelm(int nr);
extern int gotolabel(int nr);
extern void linewarnings(void);

/*-------------------------- Code generation ---------------------*/


extern void genreturns(void);
extern void gosubstmt(int lab);
extern void gotostmt(int nr);
extern void returnstmt(void);
extern void ongosubstmt(int type);
extern void ongotostmt(int type);

#endif
