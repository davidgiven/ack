#include "test.h"

/*
 * EM puts these variables in BSS.  Their initial values must be zero.
 * Some platforms, like Linux, clear the BSS before they run the
 * program.  For other platforms, like pc86, we clear the BSS in
 * boot.s before we call _m_a_i_n.
 */
char c;
int array[9000];
short s;

/* Bypasses the CRT, so there's no stdio. */
void _m_a_i_n(void)
{
	int bad, i;

	ASSERT(c == 0);
	bad = 0;
	for (i = 0; i < sizeof(array) / sizeof(array[0]); i++) {
		if(array[i])
			bad++;
	}
	ASSERT(bad == 0);
	ASSERT(s == 0);
	finished();
}
