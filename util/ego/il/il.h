/* $Id$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* I N T E R N A L   D A T A S T R U C T U R E S   O F 
 *
 *           I N L I N E   S U B S T I T U T I O N   
 *
 */


typedef struct actual	*actual_p;
typedef struct calcnt	*calcnt_p;
typedef short call_id;

struct call {
	proc_p	 cl_caller;	/* calling procedure			*/
	call_id	 cl_id;		/* uniquely denotes a CAL instruction	*/
	proc_p	 cl_proc;	/* the called procedure			*/
	byte	 cl_looplevel;	/* loop nesting level of the CAL	*/
	bool	 cl_flags;	/* flag bits				*/
	short	 cl_ratio;	/* indicates 'speed gain / size lost'	*/
	call_p	 cl_cdr;	/* link to next call			*/
	call_p	 cl_car;	/* link to nested calls			*/
	actual_p cl_actuals;	/* actual parameter expr. trees		*/
};

#define CLF_INLPARS 017		/* min(15,nr. of inline parameters) */
#define CLF_SELECTED	020	/* is call selected for expansion? */
#define CLF_EVER_EXPANDED 040	/* ever expanded? e.g. in a nested call.  */
#define CLF_FIRM	0100 	/* indicates if the call takes place in a
				 * firm block of a loop (i.e. one that
				 * is always executed, except
				 * -perhaps- at the last iteration).
				 * Used for heuristics only.
				 */

struct actual {
	line_p	 ac_exp;	/* copy of EM text			*/
				/* 0 for actuals that are not inline	*/
	offset	 ac_size;	/* number of bytes of parameter		*/
	bool	 ac_inl;	/* TRUE if it may be expanded in line	*/
	actual_p ac_next;	/* link					*/
};


struct formal {
	offset	 f_offset;	/* offsetin bytes			*/
	byte	 f_flags;	/* flags FF_BAD etc.			*/
	byte	 f_type;	/* SINGLE, DOUBLE,POINTER,UNKNOWN	*/
	formal_p f_next;	/* link					*/
};


/* flags of formal: */

#define FF_BAD		01
#define FF_REG		02
#define	FF_ONCEUSED	04
#define FF_OFTENUSED	06
#define USEMASK		014

/* types of formals: */

#define SINGLE		1
#define DOUBLE		2
#define POINTER		3
#define UNKNOWN		4

/* 'call-count' information keeps track of the number
 * of times one procedure calls another. Conceptually,
 * it may be regarded as a two dimensional array, where
 * calcnt[p,q] is the number of times p calls q. As this
 * matrix would be very dense, we use a more efficient
 * list representation. Every procedure has a list
 * of calcnt structs.
 */

struct calcnt {
	proc_p	cc_proc;	/* the called procedure */
	short	cc_count;	/* # times proc. is called in the
				 * original text of the caller.
				 */
	calcnt_p cc_next;	/* link			*/
};




extern int complete_program;
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

/* extra core-allocation macros */

#define newcall()	(call_p) newstruct(call)
#define newactual()	(actual_p) newstruct(actual)
#define newformal()	(formal_p) newstruct(formal)
#define newcalcnt()	(calcnt_p) newstruct(calcnt)
#define newilpx()	(pext_p) newstruct(pext_il)

#define oldcall(x)	oldstruct(call,x)
#define oldactual(x)	oldstruct(actual,x)
#define oldformal(x)	oldstruct(formal,x)
#define oldcalcnt(x)	oldstruct(calcnt,x)
#define oldilpx(x)	oldstruct(pext_il,x)
