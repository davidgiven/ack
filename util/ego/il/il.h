/* I N T E R N A L   D A T A S T R U C T U R E S   O F 
 *
 *           I N L I N E   S U B S T I T U T I O N   
 *
 */


extern int calnr;
extern calcnt_p cchead;		/* calcnt info of current proc */

/* Macro's for extended data structures */

#define P_CALS		p_extend->px_il.p_cals
#define P_SIZE		p_extend->px_il.p_size
#define P_FORMALS	p_extend->px_il.p_formals
#define P_NRCALLED	p_extend->px_il.p_nrcalled
#define P_CCADDR	p_extend->px_il.p_ccaddr
#define P_LADDR		p_extend->px_il.p_laddr
#define P_ORGLABELS	p_extend->px_il.p_orglabels
#define P_ORGLOCALS	p_extend->px_il.p_orglocals

/* flags2: */

#define PF_UNSUITABLE	01
#define PF_NO_INLPARS	02
#define PF_FALLTHROUGH	04
#define PF_DISPENSABLE	010
#define PF_CHANGED	020


/* kinds of usages: */

#define USE	0
#define CHANGE	1
#define ADDRESS 2




/* We do not expand calls if:
 * - the called procedure has to many local variables
 * - the calling procedure is already very large
 * - the called procedure is to large.
 */

#define MANY_LOCALS(p)	(p->p_localbytes > LOCAL_THRESHOLD)
#define LOCAL_THRESHOLD		200
#define BIG_CALLER(p)	(p->P_SIZE > CALLER_THRESHOLD)
#define CALLER_THRESHOLD	500
#define BIG_PROC(p)	(p->P_SIZE > CALLEE_THRESHOLD)
#define CALLEE_THRESHOLD	100

#define FALLTHROUGH(p)	(p->p_flags2 & PF_FALLTHROUGH)
#define DISPENSABLE(p)	p->p_flags2 |= PF_DISPENSABLE
#define IS_DISPENSABLE(p) (p->p_flags2 & PF_DISPENSABLE)
#define SELECTED(c)	c->cl_flags |= CLF_SELECTED
#define IS_SELECTED(c)	(c->cl_flags & CLF_SELECTED)
#define EVER_EXPANDED(c) c->cl_flags |= CLF_EVER_EXPANDED
#define IS_EVER_EXPANDED(c) (c->cl_flags & CLF_EVER_EXPANDED)
#define UNSUITABLE(p)	p->p_flags2 |= PF_UNSUITABLE
#define SUITABLE(p)	(!(p->p_flags2&PF_UNSUITABLE))
#define INLINE_PARS(p)	(!(p->p_flags2&PF_NO_INLPARS))
#define PARAMS_UNKNOWN(p) (p->p_nrformals == UNKNOWN_SIZE)

extern int Ssubst;
#ifdef VERBOSE
extern int Senv,Srecursive,Slocals,Sinstrlab,Sparsefails,Spremoved,Scals;
extern int Sbig_caller,Sdispensable,Schangedcallee,Sbigcallee,Sspace,Szeroratio;
#endif
