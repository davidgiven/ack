#include <setjmp.h>
#include "test.h"

/*
 * Sets i = 2 * i for each i in nums, until i == 0, but stops if
 * 2 * i >= 1000.
 *
 * Uses setjmp() and longjmp() in libc.  For ACK's libc, the back end
 * must provides EM's _gto_, and _gto_ must preserve the function
 * return area.
 */
int nums1[]         = { 79, 245, 164, 403, 0};
const int expect1[] = {158, 490, 328, 806, 0};
int nums2[]         = {20, 221, 411, 643, 48, 272, 448, 0};
const int expect2[] = {40, 442, 822, 643, 48, 272, 448, 0};
int nums3[]         = {371, 265, 500, 124, 117, 0};
const int expect3[] = {742, 530, 500, 124, 117, 0};
int docount = 0;

int twice(int i, jmp_buf esc) {
	if (i >= 500)
		longjmp(esc, i);
	return 2 * i;
}

void donums(int *nums, jmp_buf esc) {
	int *p;

	docount++;
	for (p = nums; *p != 0; p++) {
		*p = twice(*p, esc);
	}
}

int cknums(int *nums, const int *expect) {
	jmp_buf env;
	int ret;

	ret = setjmp(env);
	if (ret == 0)
		donums(nums, env);
	for (;;) {
		ASSERT(*nums == *expect);
		if (*expect == 0)
			break;
		nums++;
		expect++;
	}
	return ret;
}

int main(void) {
	ASSERT(cknums(nums1, expect1) == 0);
	ASSERT(cknums(nums2, expect2) == 643);
	ASSERT(cknums(nums3, expect3) == 500);
	ASSERT(docount == 3);
	finished();
}
