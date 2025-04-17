#include "test.h"

/*
 * Test division and remainder.  Failure code will look like
 *  - 0x3d = id 0x3, 'd' for division
 *  - 0x3e = id 0x3, 'e' for remainder
 */

struct s_divrem {
	unsigned int id;
	long long a;
	long long b;
	long long a_div_b; /* a / b */
	long long a_rem_b; /* a % b */
} s_cases[] = {
	{0x1,  310LL,  100LL,  3LL,  10LL},
	{0x2,  310LL, -100LL, -3LL,  10LL},
	{0x3, -310LL,  100LL, -3LL, -10LL},
	{0x4, -310LL, -100LL,  3LL, -10LL},
	{0x5,  3000000000000010LL,  100LL,  30000000000000LL,  10LL},
	{0x6,  3000000000000010LL, -100LL, -30000000000000LL,  10LL},
	{0x7, -3000000000000010LL,  100LL, -30000000000000LL, -10LL},
	{0x8, -3000000000000010LL, -100LL,  30000000000000LL, -10LL},
	{0x9,  3000000000000010LL,  1000000000000LL,  3000LL,  10LL},
	{0xa,  3000000000000010LL, -1000000000000LL, -3000LL,  10LL},
	{0xb, -3000000000000010LL,  1000000000000LL, -3000LL, -10LL},
	{0xc, -3000000000000010LL, -1000000000000LL,  3000LL, -10LL},
	/*
	 * In next 3 cases, i386 tries (a / (b >> 13)) >> 13 = 8,
	 * may need to correct the quotient from 8 to 7.
	 */
	{0x11, 0x864200000000LL, 0x10c840000000LL, 8LL, 0LL},
	{0x12, 0x864200000000LL, 0x10c840000001LL, 7LL, 0x10c83ffffff9LL},
	{0x13, 0x864200000000LL, 0x10c840001fffLL, 7LL, 0x10c83fff2007LL},
};

struct u_divrem {
	unsigned int id;
	unsigned long long a;
	unsigned long long b;
	unsigned long long a_div_b;
	unsigned long long a_rem_b;
} u_cases[] = {
	{0x81, 310ULL, 100ULL, 3ULL, 10ULL},
	{0x82, 3000000000000010ULL, 100ULL, 30000000000000ULL, 10ULL},
	{0x83, 3000000000000010ULL, 1000000000000ULL, 3000ULL, 10ULL},
	{0x91, 0x8000000000000000ULL, 3ULL, 0x2aaaaaaaaaaaaaaaULL, 2ULL},
	{0x92, 0xffffffffffffffffULL, 3ULL, 0x5555555555555555ULL, 0ULL},
};

#define LEN(ary) (sizeof(ary) / sizeof(ary[0]))

void _m_a_i_n(void) {
	int i;

	for (i = 0; i < LEN(s_cases); i++) {
		struct s_divrem *s = &s_cases[i];
		if (s->a / s->b != s->a_div_b)
			fail((s->id << 4) | 0xd);
		if (s->a % s->b != s->a_rem_b)
			fail((s->id << 4) | 0xe);
	}
	for (i = 0; i < LEN(u_cases); i++) {
		struct u_divrem *u = &u_cases[i];
		if (u->a / u->b != u->a_div_b)
			fail((u->id << 4) | 0xd);
		if (u->a % u->b != u->a_rem_b)
			fail((u->id << 4) | 0xe);
	}
	finished();
}
