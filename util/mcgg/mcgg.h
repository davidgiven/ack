#ifndef MCGG_H
#define MCGG_H

/* Excruciating macro which packs ir opcodes and sizes into an int for iburg's benefit.
 *
 * Sizes are mapped as: 0=1, 1=1, 2=2, 4=3, 8=4.
 */
#define ir_to_esn(iropcode, size) \
	((iropcode)*4 + \
		(((size) == 4) ? 2 : \
		 ((size) == 8) ? 3 : \
		 ((size) == 0) ? 0 : \
		  (size-1)))


#define STATE_TYPE void*
typedef struct ir* NODEPTR_TYPE;

#define STATE_LABEL(p) ((p)->state_label)

extern void* burm_label(struct ir* ir);
extern int burm_rule(void* state, int goalnt);
extern const char* burm_string[];
extern const short *burm_nts[];
extern struct ir** burm_kids(struct ir* p, int eruleno, struct ir* kids[]);
extern void burm_trace(struct ir* p, int ruleno, int cost, int bestcost);

#endif

/* vim: set sw=4 ts=4 expandtab : */
