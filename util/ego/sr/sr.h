/* I N T E R N A L   D A T A S T R U C T U R E S   O F
 *
 *       S T R E N G T H   R E D U C T I O N
 *
 */


#define LOAD 0
#define STORE 1

#define DLINK(l1,l2)	l1->l_next=l2; l2->l_prev=l1

#define same_local(l1,l2)	(off_set(l1) == off_set(l2))

#define LP_BLOCKS	lp_extend->lpx_sr.lpx_blocks
#define LP_DONE		lp_extend->lpx_sr.lpx_done
#define LP_HEADER	lp_extend->lpx_sr.lpx_header
#define LP_INSTR	lp_extend->lpx_sr.lpx_instr

/* Parameters to be provided by environment: */

extern bool ovfl_harmful;  /* Does overflow during multiplication
			    * cause a trap ?
			    */
extern bool arrbound_harmful;  /* Is it harmful to take the address of
				* a non-existing array element ?
				*/
extern int Ssr;  /* #optimizations found */
