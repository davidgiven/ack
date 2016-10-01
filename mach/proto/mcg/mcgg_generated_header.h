#include "mcg.h"
#include "mcgg.h"


#define PANIC printf

#define OP_LABEL(p) burm_calculate_label(p)
#define LEFT_CHILD(p) ((p)->left)
#define RIGHT_CHILD(p) ((p)->right)
	 
#define burm_assert(b, s) assert(b)

extern int burm_calculate_label(struct ir* ir);
extern void burm_panic_cannot_match(struct ir* ir);

static bool burm_predicate_int(struct ir* ir)
{
	return ir->goal_no == 3;
}

static bool burm_predicate_float(struct ir* ir)
{
	return ir->goal_no == 5;
}

