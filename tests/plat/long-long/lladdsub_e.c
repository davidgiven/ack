#include "test.h"

/*
 * Failure code will look like
 *  -  0x3e =  id 0x3, 'e' or 'f' for negation
 *  - 0x43a = id 0x43, 'a' for addition
 *  - 0x43b = id 0x43, 'b' for subtraction
 */

struct neg {
	unsigned int id;
	long long a;
	long long neg_a; /* -a */
} negations[] = {
	{0x1, 0LL, 0LL},
	{0x2, 2LL, -2LL},
	{0x3, -446020022096LL, 446020022096LL},
};

struct s_addsub {
	unsigned int id;
	long long a;
	long long b;
	long long a_add_b; /* a + b */
	long long a_sub_b; /* a - b */
} s_cases[] = {
	{0x41, 2LL, 1LL, 3LL, 1LL},
	{0x42, 2LL, -1LL, 1LL, 3LL},
	{0x43, 1LL, 2LL, 3LL, -1LL},
	/* a + b overflows 32 bits */
	{0x44, 0xa0000000LL, 0x60000000LL, 0x100000000LL, 0x40000000LL},
	{0x45, 1930610480LL, 842500503LL, 2773110983LL, 1088109977LL},
	/* a + b doesn't carry to bit 32; a - b does borrow from bit 32 */
	{0x51, 0x100000000LL, 0x50000000LL, 0x150000000LL, 0xb0000000LL},
	{0x52, -446020022096LL, 1037107331549LL,
	 591087309453LL, -1483127353645LL},
	/* a + b does carry to bit 32; a - b doesn't borrow from bit 32 */
	{0x53, 0x3e0000000LL, 0x20000000LL, 0x400000000LL, 0x3c0000000LL},
	{0x54, -180587215220LL, 249361198573LL,
	 68773983353LL, -429948413793LL},
	/* a + b does carry to bit 32; a - b does borrow from bit 32 */
	{0x55, 0x370000000LL, 0x90000000LL, 0x400000000LL, 0x2e0000000LL},
	{0x56, -737537585551LL, -847060446507LL,
	 -1584598032058LL, 109522860956LL},
};

struct u_addsub {
	unsigned int id;
	unsigned long long a;
	unsigned long long b;
	unsigned long long a_add_b;
	unsigned long long a_sub_b;
} u_cases[] = {
	{0x81, 2ULL, 1ULL, 3ULL, 1ULL},
	/* a + b overflows 63 bits */
	{0x82, 6092994517831567942ULL, 3716888886436146324ULL,
	 9809883404267714266ULL, 2376105631395421618ULL},
};

#define LEN(ary) (sizeof(ary) / sizeof(ary[0]))

void _m_a_i_n(void) {
	int i;

	for (i = 0; i < LEN(negations); i++) {
		struct neg *n = &negations[i];
		if (n->a != -n->neg_a)
			fail((n->id << 4) | 0xe);
		if (-n->a != n->neg_a)
			fail((n->id << 4) | 0xf);
	}
	for (i = 0; i < LEN(s_cases); i++) {
		struct s_addsub *s = &s_cases[i];
		if (s->a + s->b != s->a_add_b)
			fail((s->id << 4) | 0xa);
		if (s->a - s->b != s->a_sub_b)
			fail((s->id << 4) | 0xb);
	}
	for (i = 0; i < LEN(u_cases); i++) {
		struct u_addsub *u = &u_cases[i];
		if (u->a + u->b != u->a_add_b)
			fail((u->id << 4) | 0xa);
		if (u->a - u->b != u->a_sub_b)
			fail((u->id << 4) | 0xb);
	}
	finished();
}
