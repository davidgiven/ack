/* $Id$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/*
 * S T A C K   M O D U L E
 */
#include "../share/types.h"
#include "../share/global.h"
#include "../share/debug.h"
#include "../share/aux.h"
#include "cs.h"
#include "cs_aux.h"

#define STACK_DEPTH	250

STATIC struct token	Stack[STACK_DEPTH];
STATIC token_p		free_token;

#define Delete_top()	{--free_token; }
#define Empty_stack()	{free_token = &Stack[0]; }
#define Stack_empty()	(free_token == &Stack[0])
#define Top		(free_token - 1)

void Push(token_p tkp)
{
	if (tkp->tk_size == UNKNOWN_SIZE) {
		Empty_stack(); /* The contents of the Stack is useless. */
	} else {
		assert(free_token < &Stack[STACK_DEPTH]);

		free_token->tk_vn = tkp->tk_vn;
		free_token->tk_size = tkp->tk_size;
		(free_token++)->tk_lfirst = tkp->tk_lfirst;
	}
}

#define WORD_MULTIPLE(n)	((n / ws) * ws + ( n % ws ? ws : 0 ))

void Pop(token_p tkp, offset size)
{
	/* Pop a token with given size from the valuenumber stack into tkp. */

	/* First simple case. */
	if (size != UNKNOWN_SIZE && !Stack_empty() && size == Top->tk_size) {
		tkp->tk_vn = Top->tk_vn;
		tkp->tk_size = size;
		tkp->tk_lfirst = Top->tk_lfirst;
		Delete_top();
		return;
	}
	/* Now we're in trouble: we must pop something that is not there!
	 * We just put a dummy into tkp and pop tokens until we've
	 * popped size bytes.
	 */
	/* Create dummy. */
	tkp->tk_vn = newvalnum();
	tkp->tk_lfirst = (line_p) 0;

	/* Now fiddle with the Stack. */
	if (Stack_empty()) return;
	if (size == UNKNOWN_SIZE) {
		Empty_stack();
		return;
	}
	if (size > Top->tk_size) {
		while (!Stack_empty() && size >= Top->tk_size) {
			size -= Top->tk_size;
			Delete_top();
		}
	}
	/* Now Stack_empty OR size < Top->tk_size. */
	if (!Stack_empty()) {
		if (Top->tk_size - size < ws) {
			Delete_top();
		} else {
			Top->tk_vn = newvalnum();
			Top->tk_size -= WORD_MULTIPLE(size);
		}
	}
}

void Dup(line_p lnp)
{
	/* Duplicate top bytes on the Stack. */

	register token_p bottom = Top;
	register token_p oldtop = Top;
	register offset	nbytes = off_set(lnp);
	struct token dummy;

	/* Find the bottom of the bytes to be duplicated.
	 * It is possible that we cannot find it.
	 */
	while (bottom > &Stack[0] && bottom->tk_size < nbytes) {
		nbytes -= bottom->tk_size;
		bottom--;
	}

	if (bottom < &Stack[0]) {
		/* There was nothing. */
		dummy.tk_vn = newvalnum();
		dummy.tk_size = nbytes;
		dummy.tk_lfirst = lnp;
		Push(&dummy);
	} else {
		if (bottom->tk_size < nbytes) {
			/* Not enough, bottom == &Stack[0]. */
			dummy.tk_vn = newvalnum();
			dummy.tk_size = nbytes - bottom->tk_size;
			dummy.tk_lfirst = lnp;
			Push(&dummy);
		} else if (bottom->tk_size > nbytes) {
			/* Not integral # tokens. */
			dummy.tk_vn = newvalnum();
			dummy.tk_size = nbytes;
			dummy.tk_lfirst = lnp;
			Push(&dummy);
			bottom++;
		}
		/* Bottom points to lowest token to be dupped. */
		while (bottom <= oldtop) {
			Push(bottom++);
			Top->tk_lfirst = lnp;
		}
	}
}

void clr_stack(void)
{
	free_token = &Stack[0];
}
