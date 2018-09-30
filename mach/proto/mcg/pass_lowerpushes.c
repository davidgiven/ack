#include "mcg.h"

#if defined MCGG_OPTION_LOWER_PUSHES_TO_LOADS_AND_STORES

/* On architectures which can't push and pop cheaply, a push typically
 *
 *   sub sp, sp, -4
 *   sw r5, 0(sp)
 *
 * This is hugely wasteful when you want to push or multiple things
 * at once, which happens a lot because that's how the procedure calling
 * convention works. This code detects these runs and turns them into a
 * single stack adjustment and then offsetted accesses via the stack
 * pointer. In order to be efficient, the table needs to know how to
 * handle this efficiently:
 *
 *   STACKADJUST(CONST(-4))
 *   STORE.I(ADD.I(GETSP(), CONST(0)))
 *
 * ...otherwise the code will be *even worse*.
 *
 * We have to be careful, though, because after we do the adjustment,
 * the physical stack pointer won't match em's idea of the stack pointer
 * until the last 'push' happens. So we need to check that this is never
 * used.
 *
 * With this option set, PUSH will never be seen by the instruction
 * selector.
 */

static struct basicblock* current_bb;
static int cursor;

static bool accesses_stack_pointer_cb(struct ir* ir, void* user)
{
	switch (ir->opcode)
	{
		case IR_SETSP:
		case IR_GETSP:
		case IR_CALL:
		case IR_STACKADJUST:
			return true;

		default:
			return false;
	}
}

static bool accesses_stack_pointer(struct ir* ir)
{
	return !!ir_walk(ir, accesses_stack_pointer_cb, NULL);
}

static void consider_push(struct ir* ir)
{
	int runstart;
	int delta;
	int i;

	if (ir->opcode != IR_PUSH)
	{
		cursor++;
		return;
	}

	/* This is the first push of a run; we'll want to insert the STACKADJUST
	 * before this one. */

	tracef('P', "found push in %s at IR index %d\n", current_bb->name, cursor);
	runstart = cursor;

	/* Now start walking forward until we find an IR which isn't a safe push.
	 * The current IR is always safe. */

	for (;;)
	{
		struct ir* ir;

		cursor++;
		if (cursor == current_bb->irs.count)
			break;

		ir = current_bb->irs.item[cursor];
		if (ir->opcode != IR_PUSH)
			break;
		if (accesses_stack_pointer(ir))
			break;

		delta += ir->size;
	}

	tracef('P', "found end of run at IR index %d\n", cursor);

	/* Now work backwards, converting each push into a stack write. */

	delta = 0;
	i = cursor - 1;
	while (i >= runstart)
	{
		struct ir* ir = current_bb->irs.item[i];
		struct ir* value_ir = ir->left;
		assert(ir->opcode == IR_PUSH);

		ir->opcode = IR_STORE;
		ir->left = new_ir2(
			IR_ADD, EM_pointersize,
			new_ir0(IR_GETSP, EM_pointersize),
			new_wordir(delta)
		);
		ir->left->root = ir->left->left->root = ir->left->right->root = ir->root;
		ir->left->bb = ir->left->left->bb = ir->left->right->bb = current_bb;
		ir->right = value_ir;

		delta += ir->size;
		i--;
	}

	/* And finally, before the place where the first push was, adjust the
	 * stack. */

	ir = new_ir1(IR_STACKADJUST, EM_pointersize, new_wordir(-delta));
	ir->left->root = ir->root = ir;
	ir->left->bb = ir->bb = current_bb;
	array_insert(&current_bb->irs, ir, runstart);
	cursor++;
}

void pass_lower_pushes(void)
{
    int i;

	for (i=0; i<current_proc->blocks.count; i++)
    {
        current_bb = current_proc->blocks.item[i];

		cursor = 0;
		while (cursor < current_bb->irs.count)
			consider_push(current_bb->irs.item[cursor]);
    }
}

#endif
