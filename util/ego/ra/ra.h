/* $Id$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/*
 *   R E G I S T E R    A L L O C A T I O N
 *
 */

#define INFINITE	10000
#define NRREGTYPES	(reg_float+1)

extern int nrinstrs;  /* number of instructions of current procedure */
extern line_p *instrmap; 
/* Dynamic array: instrmap[i] points to i'th instruction */

extern cond_p alocaltab[NRREGTYPES][NRREGTYPES],
	alocaddrtab[NRREGTYPES][NRREGTYPES], aconsttab,
	adconsttab,aglobaltab,aproctab;
extern cond_p olocaltab[NRREGTYPES],olocaddrtab[NRREGTYPES],
	oconsttab,odconsttab,oglobaltab,oproctab;
extern cond_p regsav_cost;

/* Register Allocation */
typedef struct item *item_p;
typedef struct allocation *alloc_p;
typedef struct interval *interv_p;
typedef struct time *time_p;




extern short regs_available[];  /* contains #registers of every type */
extern short use_any_as_pointer;/* indicates whether general registers
				   can be used as pointers
				*/


/* A thing that can be put in a register is called an "item". The are several
 * types of items: a local variable, the address of a local variable,
 * the address of a global variable, the address of a procedure,
 * a word-size constant and a doubleword- size constant.
 */

#define LOCALVAR	0
#define LOCAL_ADDR	1
#define GLOBL_ADDR	2
#define PROC_ADDR	3
#define CONST		4
#define DCONST		5

#define NO_ITEM		6
#define NRITEMTYPES	6

struct item {
	item_p	  it_next;	/* link to next item is list		*/
	short	  it_type;	/* its type; see above			*/
	short	  it_regtype;	/* preferred type of register		*/
	short	  it_size;	/* its size (in bytes)			*/
	short	  it_lastlive;	/* temporary, used to build livetime	*/
	lset	  it_usage;	/* all points in text where item is used*/
	interv_p  it_lives;	/* intervals during which item is live	*/
	bool	  it_desirable; /* should this item be put in reg.?	*/
	union {
		obj_p	it_obj;		/* for GLOBL_ADDR 		*/
		proc_p	it_proc;	/* for PROC_ADDR		*/
		offset	it_off;		/* for others	  		*/
	} i_t;
};


/* A 'point in time' is defined by a (line,basic block) pair */

struct time {
	line_p	  t_line;	/* point in EM text			*/
	bblock_p  t_bblock;	/* its basic block			*/
};


struct interval {
	short	 i_start;	/* number of first instruction		*/
	short	 i_stop;	/* number of last instruction		*/
	interv_p i_next;
};


/* An item may be put in a register for the duration of a whole procedure
 * or part of a procedure (e.g. a loop). So a possible "allocation" looks
 * like: put item X in a register during the timespan T (which is a subset
 * of the timespan of the entire procedure). The packing process deals
 * with allocations, rather than items. One item may be part of several
 * possible allocations.
 */

struct allocation {
	item_p	  al_item;	/* the item to be put in a register	  */
	short	  al_id;	/* unique identifying number		  */
	short	  al_regtype;	/* the register type to be used		  */
	interv_p  al_timespan;	/* timespan during which item is in reg.  */
	short	  al_profits;	/* gains of putting item in register	  */
	cset	  al_rivals;	/* set of allocations competing with it	  */
	short	  al_susecount;	/* #usages during timespan (statically)   */
	short	  al_dusecount;	/* #usages (dynamically, estimate)	  */
	lset	  al_inits;	/* points where reg. must be initialized  */
	interv_p  al_busy;	/* used to compute rivals		  */
	short	  al_regnr;	/* register nr.,if it is granted a reg.   */
	offset	  al_dummy;	/* dummy local variable,if granted a reg  */
	alloc_p	  al_mates;	/* link to allocations packed in same reg */
	alloc_p	  al_wholeproc;	/* alloc. for whole proc as timespan  	  */
	short	  al_cntrivals; /* # unpacked rivals ; used for cost estim. */
	bool	  al_isloop;	/* true if timespan consists of loop	  */
	bool	  al_iswholeproc;/*true if timespan consists of whole proc*/
	alloc_p	  al_next;	/* link to next one in a list		  */
};

extern short alloc_id;  /* last al_id used for current procedure */

#define LP_BLOCKS	lp_extend->lpx_ra.lpx_blocks
#define LP_HEADER	lp_extend->lpx_ra.lpx_header
#define B_BEGIN		b_extend->bx_ra.bx_begin
#define B_END		b_extend->bx_ra.bx_end

#define DLINK(l1,l2)	l1->l_next=l2; l2->l_prev=l1

struct item_descr {
	int	id_type;
	int	id_replindex;
} ;

extern struct item_descr itemtab[];

#define newalloc()	(alloc_p) newstruct(allocation)
#define  oldalloc(a)	oldstruct(allocation,a)
#define newitem()	(item_p) newstruct(item)
#define olditem(i)	oldstruct(item,i)
#define newtime()	(time_p) newstruct(time)
#define oldtime(t)	oldstruct(time,t)
#define newinterval()	(interv_p) newstruct(interval)
#define oldinterval(i)	oldstruct(interval,i)
