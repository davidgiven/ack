#include "mcg.h"
#include "mcgg.h"


#define PANIC printf

static int OP_LABEL(struct ir* ir)
{
	if (ir->is_generated)
	{
		assert(ir->is_sequence);
		return ir_to_esn(IR_REG, ir->size);
	}
	return ir_to_esn(ir->opcode, ir->size);
}

#define LEFT_CHILD(p) ((p)->left)
#define RIGHT_CHILD(p) ((p)->right)
	 
#define burm_assert(b, s) assert(b)

extern void burm_panic_cannot_match(struct ir* ir);

