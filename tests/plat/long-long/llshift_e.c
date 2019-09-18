#include "test.h"

/*
 * i << 1 is a constant shift.  i << (1 + zero) is a variable shift,
 * and may use a different rule in some code generators.
 */
int zero = 0;

long long i = 121LL;
long long j = 224690292230LL;
unsigned long long u = 12022195707510591570ULL;

void _m_a_i_n(void) {
	ASSERT(i << 0 == 121LL);
	ASSERT(i << (0 + zero) == 121LL);
	ASSERT(i << 1 == 242LL);
	ASSERT(i << (1 + zero) == 242LL);
	ASSERT(i << 26 == 8120172544LL);
	ASSERT(i << (26 + zero) == 8120172544LL);
	ASSERT(i << 32 == 519691042816LL);
	ASSERT(i << (32 + zero) == 519691042816LL);
	ASSERT(i << 56 == 8718968878589280256LL);
	ASSERT(i << (56 + zero) == 8718968878589280256LL);

	ASSERT(i >> 0 == 121LL);
	ASSERT(i >> (0 + zero) == 121LL);
	ASSERT(i >> 1 == 60LL);
	ASSERT(i >> (1 + zero) == 60LL);
	ASSERT(i >> 7 == 0LL);
	ASSERT(i >> (7 + zero) == 0LL);
	ASSERT(i >> 37 == 0LL);
	ASSERT(i >> (37 + zero) == 0LL);

	ASSERT(-i >> 0 == -121LL);
	ASSERT(-i >> (0 + zero) == -121LL);
	ASSERT(-i >> 1 == -61LL);
	ASSERT(-i >> (1 + zero) == -61LL);
	ASSERT(-i >> 7 == -1LL);
	ASSERT(-i >> (7 + zero) == -1LL);
	ASSERT(-i >> 37 == -1LL);
	ASSERT(-i >> (37 + zero) == -1LL);

	ASSERT(j << 0 == 224690292230LL);
	ASSERT(j << (0 + zero) == 224690292230LL);
	ASSERT(j << 10 == 230082859243520LL);
	ASSERT(j << (10 + zero) == 230082859243520LL);
	ASSERT(j << 25 == 7539355131691663360LL);
	ASSERT(j << (25 + zero) == 7539355131691663360LL);

	ASSERT(j >> 0 == 224690292230LL);
	ASSERT(j >> (0 + zero) == 224690292230LL);
	ASSERT(j >> 6 == 3510785816LL);
	ASSERT(j >> (6 + zero) == 3510785816LL);
	ASSERT(j >> 32 == 52LL);
	ASSERT(j >> (32 + zero) == 52LL);
	ASSERT(j >> 38 == 0LL);
	ASSERT(j >> (38 + zero) == 0LL);

	ASSERT(-j >> 0 == -224690292230LL);
	ASSERT(-j >> (0 + zero) == -224690292230LL);
	ASSERT(-j >> 6 == -3510785817LL);
	ASSERT(-j >> (6 + zero) == -3510785817LL);
	ASSERT(-j >> 32 == -53LL);
	ASSERT(-j >> (32 + zero) == -53LL);
	ASSERT(-j >> 38 == -1LL);
	ASSERT(-j >> (38 + zero) == -1LL);

	ASSERT(u >> 0 == 12022195707510591570ULL);
	ASSERT(u >> (0 + zero) == 12022195707510591570ULL);
	ASSERT(u >> 1 == 6011097853755295785ULL);
	ASSERT(u >> (1 + zero) == 6011097853755295785ULL);
	ASSERT(u >> 32 == 2799135564ULL);
	ASSERT(u >> (32 + zero) == 2799135564ULL);
	ASSERT(u >> 41 == 5467061ULL);
	ASSERT(u >> (41 + zero) == 5467061ULL);

	finished();
}
