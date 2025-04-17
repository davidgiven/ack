#include "test.h"

long long a = 40000LL;
long long b = 3000000000LL;
long long c = 200000000000000LL;
unsigned long long d = 60000ULL;

/* products a * b, a * c, c * d */
long long ab = 120000000000000LL;
long long ac = 8000000000000000000LL;
unsigned long long cd = 12000000000000000000ULL;

void _m_a_i_n(void) {
	ASSERT(a * b == ab);
	ASSERT(-b * a == -ab);
	ASSERT(b * -40000LL == -ab);
	ASSERT(c * a == ac);
	ASSERT(a * -c == -ac);
	ASSERT(40000LL * -c == -ac);
	ASSERT(c * d == cd);
	ASSERT(d * c == cd);
	finished();
}
