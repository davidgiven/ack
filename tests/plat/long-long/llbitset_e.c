#include "test.h"

typedef unsigned long long set;

set a       = 0xfaab001bd86b595aLL;
set b       = 0x3abe6373562dec1cLL;
set not_a   = 0x0554ffe42794a6a5LL;
set a_and_b = 0x3aaa001350294818LL;
set a_or_b  = 0xfabf637bde6ffd5eLL;
set a_xor_b = 0xc01563688e46b546LL;

/* ACK C evaluates H(constant) at compile time. */
#define H(x) ((set)x << 32)

void _m_a_i_n(void) {
	ASSERT((~a & 0xffffffffffffffffLL) == not_a);
	ASSERT((a & b) == a_and_b);
	ASSERT((a | b) == a_or_b);
	ASSERT((a ^ b) == a_xor_b);

	ASSERT((a & 1)     == 0);
	ASSERT((2 & a)     == 2);
	ASSERT((a & ~8)    == 0xfaab001bd86b5952LL);
	ASSERT((a & H(1))  == H(1));
	ASSERT((H(4) & a)  == 0);
	ASSERT((a & ~H(2)) == 0xfaab0019d86b595aLL);

	ASSERT((a | 1)     == 0xfaab001bd86b595bLL);
	ASSERT((2 | a)     == a);
	ASSERT((a | H(4))  == 0xfaab001fd86b595aLL);
	ASSERT((H(8) | a)  == a);

	ASSERT((a ^ 1)     == 0xfaab001bd86b595bLL);
	ASSERT((2 ^ a)     == 0xfaab001bd86b5958LL);
	ASSERT((a ^ H(4))  == 0xfaab001fd86b595aLL);
	ASSERT((H(8) ^ a)  == 0xfaab0013d86b595aLL);

	finished();
}

