/* $Id$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* I N T E R N A L   D A T A S T R U C T U R E S   O F
 *
 *       S T R E N G T H   R E D U C T I O N
 *
 */

typedef struct iv	*iv_p;
typedef struct code_info *code_p;

/* An induction variable */

struct iv {
	offset	 iv_off;	/* offset of the induction variable */
	line_p	 iv_incr;	/* pointer to last instr. of EM-code that
				 * increments the induction variable	*/
	offset	 iv_step;	/* step value				*/
};


/* All information about a reducible piece of code is collected in
 * a single structure.
 */

struct code_info {
	loop_p	  co_loop;	/* the loop the code is in		*/
	bblock_p  co_block;	/* the basic block the code is in	*/
	line_p	  co_lfirst;	/* first instruction of the code	*/
	line_p	  co_llast;	/* last  instruction of the code	*/
	line_p	  co_ivexpr;	/* start of linear expr. using iv	*/
	line_p	  co_endexpr;	/* end of the expression		*/
	int	  co_sign;	/* sign of iv in above expr		*/
	iv_p	  co_iv;	/* the induction variable		*/
	offset	  co_temp;	/* temporary variable			*/
	int	  co_tmpsize;	/* size of the temp. variable (ws or ps)*/
	int	  co_instr;	/* the expensive instruction (mli,lar..)*/
	union {
		line_p	co_loadlc;	/* LOC lc instruction (for mult.)*/
		line_p  co_desc;	/* load address of descriptor
					 *   (for lar etc.)		*/
	} c_o;
};

#define LOAD 0
#define STORE 1

#define DLINK(l1,l2)	l1->l_next=l2; l2->l_prev=l1

#define same_local(l1,l2)	(off_set(l1) == off_set(l2))

#define LP_BLOCKS	lp_extend->lpx_sr.lpx_blocks
#define LP_DONE		lp_extend->lpx_sr.lpx_done
#define LP_HEADER	lp_extend->lpx_sr.lpx_header
#define LP_INSTR	lp_extend->lpx_sr.lpx_instr

/* Parameters to be provided by environment: */

extern int ovfl_harmful;  /* Does overflow during multiplication
			    * cause a trap ?
			    */
extern int arrbound_harmful;  /* Is it harmful to take the address of
				* a non-existing array element ?
				*/
extern int sli_threshold; /* Try to optimize SLI if shift-count larger than
			    * this
			    */
extern int Ssr;  /* #optimizations found */

/* core allocation macros */
#define newiv()		(iv_p)		newstruct(iv)
#define newcinfo()	(code_p) 	newstruct(code_info)
#define newsrlpx()	(lpext_p) 	newstruct(lpext_sr)
#define oldiv(x)	oldstruct(iv,x)
#define oldcinfo(x)	oldstruct(code_info,x)
#define oldsrlpx(x)	oldstruct(lpext_sr,x)
