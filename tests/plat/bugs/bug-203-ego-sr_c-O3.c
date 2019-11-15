#include "test.h"

/*
 * https://github.com/davidgiven/ack/issues/203 was a bug in the
 * strength reduction (SR) phase of ego.  To cause the bug:
 *
 *  1. SR must run; it runs at `ack -O3` and higher.
 *
 *  2. SR must find a loop with an induction variable, and SR must
 *     find at least 2 different expressions using the variable, like
 *     i << 1, (i + 1) << 1, or 3 * i, 5 * i.
 *
 *  3. There is no basic block to act as a loop header, so SR must
 *     create the block.  One way is to put if (one) { ... } around
 *     the loop, so the loop is just after a conditional branch.
 */

/*
 * Global variables prevent optimizations:
 * if (one) { ... } caused the bug, if (1) { ... } didn't.
 */
int zero = 0;
int one = 1;
short ary[] = {8, 8, 8, 8, 8, 9};

int left_shift(int i) {
	if (one) {
		/* EM will use i << 1, (i + 1) << 1 here. */
		while (ary[i] == ary[i + 1])
			i++;
	}
	return i;
}

int multiply(int i) {
	if (one) {
		while (((3 * i) & (5 * i)) < 40)
			i++;
	}
	return i;
}

int main(void) {
	ASSERT(left_shift(zero) == 4);
	ASSERT(multiply(zero) == 21);
	finished();
}
