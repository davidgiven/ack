/* $Id$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* I N T E R N A L   D A T A S T R U C T U R E S   O F   E G O  */


/* This file contains the definitions of the global data types.
 */


/* TEMPORARY: */
#define LONGOFF


#define IDL 256	/* maximum identifier length */
#define DYNAMIC 1
#define NARGBYTES 14
#define BMASK 0377

typedef struct argbytes argb_t;
typedef char	byte;
typedef	byte	bool;
typedef long	offset;
typedef short	obj_id;
typedef short	proc_id;
typedef short	dblock_id;
typedef short	block_id;
typedef short	loop_id;
typedef short	lab_id;


typedef struct dblock	  *dblock_p;
typedef struct obj	  *obj_p;
typedef struct proc	  *proc_p;
typedef struct loop	  *loop_p;
typedef struct change	  *change_p;
typedef struct use	  *use_p;
typedef struct bblock	  *bblock_p;
typedef struct line	  *line_p;
typedef struct arg	  *arg_p;
typedef struct argbytes	  *argb_p;
typedef struct elemholder *elem_p;
typedef struct elemholder *lset;
typedef struct bitvector  *cset;
typedef elem_p Lindex;
typedef short  Cindex;
typedef void   *Lelem_t;
typedef short  Celem_t;

typedef union pext_t *pext_p;
typedef union bext_t *bext_p;
typedef union lpext_t *lpext_p;


typedef struct call	*call_p;
typedef struct formal	*formal_p;

/* Used-Definition Analysis */
typedef struct local *local_p;

typedef struct cond_tab *cond_p;

#define TRUE	1
#define FALSE	0

/* DATABLOCKS */

/* A datablock is a block of global data, declared by means of
 * a hol, bss, con or rom pseudo. The declaration may be in a file
 * that is inaccessible to EGO, in which case the pseudo is unknown.
 * Successive rom or con pseudos that are garanteed to be in the
 * same fragment (according to the EM definition) share the
 * same fragment number.
 */

#define DHOL	 0
#define DBSS	 1
#define DROM	 2
#define DCON	 3
#define	DUNKNOWN 4


/* The following constants are used by the debugging tools: */
#define D_FIRST DHOL
#define D_LAST  DUNKNOWN


struct dblock {
	dblock_id d_id;		/* unique integer			*/
	byte	 d_pseudo;	/* one of DHOL,DBSS,DROM,DCON,DUNKNOWN	*/
	offset	 d_size;	/* # bytes, -1 if unknown		*/
	obj_p	 d_objlist;	/* list of objects of the data block	*/
	byte	 d_flags1;	/* see below				*/
	byte	 d_flags2;	/* free to be used by phases		*/
	arg_p	 d_values;	/* values, in case of ROM		*/
	short	 d_fragmnr;	/* fragment number			*/
	dblock_p d_next;	/* link to next block			*/
};


#define DF_EXTERNAL	01	/* Is name visible outside its module? */

/* OBJECTS */

/* An object is a row of successive bytes in one datablock
 * that are considered to be a whole. E.g. scalar variables,
 * arrays, I/O buffers etc. are objects.
 */

struct obj {
	offset	 o_off;		/* offset within the block		*/
	offset	 o_size;	/* size of the object, 0 if not known	*/
	obj_id	 o_id;		/* unique integer			*/
	dblock_p o_dblock;	/* backlink to data block		*/
	short	 o_globnr;	/* global variable number		*/
	obj_p	 o_next;	/* link					*/
};


/* PROCEDURES */

struct proc {
	proc_id	 p_id;		/* unique integer			*/
	short	 p_nrlabels;	/* #instruction labels in the proc	*/
	offset	 p_localbytes;	/* #bytes for locals			*/
	offset	 p_nrformals;	/* #bytes for formals			*/
	byte	 p_flags1;	/* see below				*/
	byte	 p_flags2;	/* free to be used by phases		*/
	bblock_p p_start;	/* pointer to first basic block		*/
	cset	 p_calling;	/* set of all procs called by this one	*/
	lset	 p_loops;	/* information about loops		*/
	change_p p_change;	/* variables changed by this proc	*/
	use_p	 p_use;		/* variables used by this proc		*/
	pext_p	 p_extend;	/* pointer to any further information	*/
	proc_p	 p_next;	/* link					*/
};


union pext_t {
   struct pext_il {
	call_p	 p_cals;	/* candidate calls for in line expansion */
	short	 p_size;	/* length of proc (EM-instrs or bytes)	*/
	formal_p p_formals;	/* description of formals		*/
	short	 p_nrcalled;	/* # times proc is called (varying)	*/
	long	 p_ccaddr;	/* address of calcnt info on disk	*/
	long	 p_laddr;	/* address in EM-text file on disk	*/
	short	 p_orglabels;	/* original #labels before substitution	*/
	offset	 p_orglocals;	/* original #bytes for locals		*/
   } px_il;
} ;

#define PF_EXTERNAL	01	/* proc is externally visible */
#define PF_BODYSEEN	02	/* body of proc is available as EM text */
#define PF_CALUNKNOWN	04	/* proc calls an unavailable procedure */
#define PF_ENVIRON	010	/* proc does a lxa or lxl */
#define PF_LPI		020	/* proc may be called indirect */
#define PF_CALINLOOP	040	/* proc ever called in a loop? (transitively) */
#define PF_GTO		0100	/* proc may be entered via GTO instruction */

#define CALLED_IN_LOOP(p)	p->p_flags1 |= PF_CALINLOOP
#define IS_CALLED_IN_LOOP(p)	(p->p_flags1 & PF_CALINLOOP)
#define IS_ENTERED_WITH_GTO(p)	(p->p_flags1 & PF_GTO)
#define ENTERED_WITH_GTO(p)	p->p_flags1 |= PF_GTO
#define BODY_KNOWN(p)		(p->p_flags1 & (byte) PF_BODYSEEN)


/* LOOPS */

 struct loop {
	loop_id	 lp_id;		/* unique integer			*/
	short	 lp_level;	/* nesting level, 0=outermost loop,
				 * 1=loop within loop etc.		*/
	bblock_p lp_entry;	/* unique entry block of loop		*/
	bblock_p lp_end;	/* tail of back edge of natural loop	*/
	lpext_p	 lp_extend;	/* pointer to any further information	*/
};



union lpext_t {
   struct lpext_cf {
	lset	 lpx_blocks;
	short	 lpx_count;
	bool	 lpx_messy;
   } lpx_cf;
   struct lpext_sr {
	lset	 lpx_blocks;	/* basic blocks  constituting the loop  */
	bblock_p lpx_header;	/* header block, 0 if no one allocated yet */
	bool	 lpx_done;	/* TRUE if we've processed this loop	*/
	line_p	 lpx_instr;	/* current last instruction in header block*/
   } lpx_sr;
   struct lpext_ra {
	lset	 lpx_blocks;	/* basic blocks  constituting the loop  */
	bblock_p lpx_header;	/* header block, 0 if no one allocated yet */
   } lpx_ra;
} ;

/* CHANGED/USED VARIABLES INFORMATION */


struct change {
	cset	 c_ext;		/* external variables changed		*/
	short	 c_flags;	/* see below				*/
};

struct use {
	short	 u_flags;	/* see below				*/
};


#define CF_INDIR 01
#define UF_INDIR 01

#define CHANGE_INDIR(p)		(p->p_change->c_flags & CF_INDIR)

/* SETS */


/* There are 2 set representations:
 *   - long    (lset), which is essentially a list
 *   - compact (cset), which is essentially a bitvector
 */


 struct elemholder {
	char	   *e_elem;	/* pointer to the element		*/
	elem_p     e_next;	/* link					*/
};

struct bitvector {
	short	v_size;		/* # significant bits			*/
	int	v_bits[DYNAMIC];/* a row of bits			*/
};



/* BASIC BLOCKS */


/* Note that the b_succ and b_pred fields constitute the
 * Control Flow Graph
 */


 struct bblock {
	block_id b_id;		/* unique integer			*/
	line_p	 b_start;	/* pointer to first instruction		*/
	lset	 b_succ;	/* set of successor blocks		*/
	lset	 b_pred;	/* set of predecessor blocks		*/
	bblock_p b_idom;	/* immediate dominator			*/
	lset	 b_loops;	/* set of loops it is in		*/
	short	 b_flags;	/* see below				*/
	bext_p	 b_extend;	/* pointer to any further information	*/
	bblock_p b_next;	/* link to textually next block		*/
};


union bext_t {
   struct bext_cf {
	short	 bx_semi;	/* dfs number of semi-dominator		*/
	bblock_p bx_parent;	/* parent in dfs spanning tree		*/
	lset	 bx_bucket;	/* set of vertices whose sdom is b	*/
	bblock_p bx_ancestor;	/* ancestor of b in forest,		*/
	bblock_p bx_label;	/* used by link/eval			*/
   } bx_cf;
   struct bext_ud {
	cset	 bx_gen;	/* definition generated in b		*/
	cset	 bx_kill;	/* defs. outside b killed by b		*/
	cset	 bx_in;		/* defs. reaching beginning of b	*/
	cset	 bx_out;	/* defs. reaching end of b		*/
	cset	 bx_cgen;	/* generated copies			*/
	cset	 bx_ckill;	/* killed copies			*/
	cset	 bx_cin;	/* copies reaching begin of b		*/
	cset	 bx_cout;	/* copies reaching end of b		*/
	cset	 bx_chgvars;	/* variables changed by b		*/
   } bx_ud;
   struct bext_lv {
	cset	 bx_use;	/* variables used before being defined  */
	cset	 bx_def;	/* variables defined before being used  */
	cset	 bx_lin;	/* variables live at entry of b         */
	cset	 bx_lout;	/* variables live at exit of b          */
   } bx_lv;
   struct bext_ra {
	short	 bx_begin;	/* number of first instruction of block	*/
	short	 bx_end;	/* number of last  instruction of block	*/
   } bx_ra;
} ;


#define BF_STRONG	01
#define BF_FIRM		02

#define IS_STRONG(b)	(b->b_flags&BF_STRONG)
#define IS_FIRM(b)	(b->b_flags&BF_FIRM)


/* EM INSTRUCTIONS */

/* Kinds of operand types (l_optype field) */

#define OPNO		0
#define OPSHORT		1
#define OPOFFSET	2
#define OPINSTRLAB	3
#define OPOBJECT	4
#define OPPROC		5
#define OPLIST		6


/* The following constants are used by the debugging tools: */
#define OP_FIRST OPNO
#define OP_LAST	OPLIST

#define LDATA	0
#define LTEXT	01

struct line {
	line_p	 l_next;	/* link					*/
	byte	 l_instr;	/* instruction				*/
	byte	 l_optype;	/* kind of operand, used as tag		*/
	line_p	 l_prev;	/* backlink to previous instruction	*/
	union {
		short	la_short;	/* short:	   LOC 5	*/
		offset	la_offset;	/* offset:	   LDC 20	*/
		lab_id	la_instrlab;	/* label:	   BRA *10	*/
		obj_p	la_obj;		/* object:	   LOE X+2	*/
		proc_p	la_proc;	/* proc:	   CAL F3	*/
		arg_p	la_arg;		/* arguments:	   HOL 10,0,0	*/
	} l_a;
};


/* ARGUMENTS */


/* String representation of a constant, partitioned into
 * pieces of NARGBYTES bytes.
 */

#define ARGOFF		0
#define ARGINSTRLAB	1
#define ARGOBJECT	2
#define ARGPROC		3
#define ARGSTRING	4
#define ARGICN		5
#define ARGUCN		6
#define ARGFCN		7
#define ARGCEND		8


struct argbytes {
	argb_p	 ab_next;
	short	 ab_index;
	char	 ab_contents[NARGBYTES];
};


struct arg {
	arg_p	 a_next;	/* link					*/
	short	 a_type;	/* kind of argument			*/
	union {
		offset	a_offset;	/* offset			*/
		lab_id	a_instrlab;	/* instruction label		*/
		proc_p	a_proc;		/* procedure			*/
		obj_p	a_obj;		/* object			*/
		argb_t	a_string;	/* string			*/
		struct {		/* int/unsigned/float constant	*/
			short	ac_length;	/* size in bytes	*/
			argb_t	ac_con;		/* its string repres.	*/
		} a_con;
	} a_a;
};



/* Macros to increase readability: */

#define	INSTR(lnp)	(lnp->l_instr & BMASK)
#define TYPE(lnp)	lnp->l_optype
#define PREV(lnp)	lnp->l_prev
#define	SHORT(lnp)	lnp->l_a.la_short
#define	OFFSET(lnp)	lnp->l_a.la_offset
#define	INSTRLAB(lnp)	lnp->l_a.la_instrlab
#define	OBJ(lnp)	lnp->l_a.la_obj
#define	PROC(lnp)	lnp->l_a.la_proc
#define	ARG(lnp)	lnp->l_a.la_arg


/* Data structures for Use-Definition and Live-Dead Analysis */

struct local {
	offset	lc_off;		/* offset of local in stackframe 	*/
	short	lc_size;	/* size of local in bytes		*/
	short	lc_flags;	/* see below				*/
	offset	lc_score;	/* score in register message, if regvar */
	local_p	lc_next;	/* link, only used when building the list */
};

/* values of lc_flags */

#define LCF_BAD		01
/* Set when no ud-info for this local is maintained, e.g. when it is
 * overlapped by another local.
 */
#define LCF_REG		02	/* register variable */
#define LCF_LIVE	04	/* use by live-dead message generation */


struct cond_tab {
	short	mc_cond;	/* Denotes a condition e.g. FITBYTE */
	short	mc_tval;	/* value for time optimization  */
	short	mc_sval;	/* value for space optimization */
	short mc_dummy; /* allignment */
};

/* conditions: */

#define DEFAULT		0
#define FITBYTE		1
#define IN_0_63		2
#define IN_0_8		3

