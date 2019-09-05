#include "test.h"

struct neg {
	long long a;
	long long neg_a; /* -a */
} negations[] = {
	{0LL, 0LL},
	{2LL, -2LL},
	{-446020022096LL, 446020022096LL},
};

struct s_addsub {
	long long a;
	long long b;
	long long a_add_b; /* a + b */
	long long a_sub_b; /* a - b */
} s_cases[] = {
	{2LL, 1LL, 3LL, 1LL},
	{2LL, -1LL, 1LL, 3LL},
	/* a + b overflows 32 bits */
	{1930610480LL, 842500503LL, 2773110983LL, 1088109977LL},
	{-446020022096LL, 1037107331549LL, 591087309453LL, -1483127353645LL},
	{-737537585551LL, -847060446507LL, -1584598032058LL, 109522860956LL},
};

struct u_addsub {
	unsigned long long a;
	unsigned long long b;
	unsigned long long a_add_b;
	unsigned long long a_sub_b;
} u_cases[] = {
	{2ULL, 1ULL, 3ULL, 1ULL},
	/* a + b overflows 63 bits */
	{6092994517831567942ULL, 3716888886436146324ULL,
	 9809883404267714266ULL, 2376105631395421618ULL},
};

#define LEN(ary) (sizeof(ary) / sizeof(ary[0]))

void _m_a_i_n(void) {
	int i;

	for (i = 0; i < LEN(negations); i++) {
		struct neg *n = &negations[i];
		ASSERT(n->a == -n->neg_a);
		ASSERT(-n->a == n->neg_a);
	}
	for (i = 0; i < LEN(s_cases); i++) {
		struct s_addsub *s = &s_cases[i];
		ASSERT(s->a + s->b == s->a_add_b);
		ASSERT(s->a - s->b == s->a_sub_b);
	}
	for (i = 0; i < LEN(u_cases); i++) {
		struct u_addsub *u = &u_cases[i];
		ASSERT(u->a + u->b == u->a_add_b);
		ASSERT(u->a - u->b == u->a_sub_b);
	}
	finished();
}
