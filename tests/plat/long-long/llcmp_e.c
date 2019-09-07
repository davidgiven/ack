#include "test.h"

struct s_cmp {
	long long a;
	long long b;
	int a_cmp_b; /* -1 if a < b, 0 if a == b, 1 if a > b */
} s_cases[] = {
	{-1LL, -1LL,  0},
	{-1LL,  0LL, -1},
	{-1LL,  1LL, -1},
	{ 0LL, -1LL,  1},
	{ 0LL,  0LL,  0},
	{ 0LL,  1LL, -1},
	{ 1LL, -1LL,  1},
	{ 1LL,  0LL,  1},
	{ 1LL,  1LL,  0},
};

struct u_cmp {
	unsigned long long a;
	unsigned long long b;
	int a_cmp_b;
} u_cases[] = {
	{ 0ULL,  0ULL,  0},
	{ 0ULL,  1ULL, -1},
	{ 1ULL,  0ULL,  1},
	{ 1ULL,  1ULL,  0},
};

#define LEN(ary) (sizeof(ary) / sizeof(ary[0]))

/* Compiler should not optimize !t[a < b] as a > b. */
int t[] = {0, 1};

void _m_a_i_n(void) {
	int i;
#define A c->a
#define B c->b

	for (i = 0; i < LEN(s_cases); i++) {
		struct s_cmp *c = &s_cases[i];
		switch (c->a_cmp_b) {
		case -1:
			ASSERT(A < B);
			ASSERT(A <= B);
			ASSERT(A != B);
			ASSERT(t[A < B]);
			ASSERT(t[A <= B]);
			ASSERT(!t[A == B]);
			ASSERT(t[A != B]);
			ASSERT(!t[A >= B]);
			ASSERT(!t[A > B]);
			break;
		case 0:
			ASSERT(A <= B);
			ASSERT(A == B);
			ASSERT(A >= B);
			ASSERT(!t[A < B]);
			ASSERT(t[A <= B]);
			ASSERT(t[A == B]);
			ASSERT(!t[A != B]);
			ASSERT(t[A >= B]);
			ASSERT(!t[A > B]);
			break;
		case 1:
			ASSERT(A != B);
			ASSERT(A >= B);
			ASSERT(A > B);
			ASSERT(!t[A < B]);
			ASSERT(!t[A <= B]);
			ASSERT(!t[A == B]);
			ASSERT(t[A != B]);
			ASSERT(t[A >= B]);
			ASSERT(t[A > B]);
			break;
		default:
			ASSERT(0);
			break;
		}
	}
	for (i = 0; i < LEN(u_cases); i++) {
		struct u_cmp *c = &u_cases[i];
		switch (c->a_cmp_b) {
		case -1:
			ASSERT(A < B);
			ASSERT(A <= B);
			ASSERT(A != B);
			ASSERT(t[A < B]);
			ASSERT(t[A <= B]);
			ASSERT(!t[A == B]);
			ASSERT(t[A != B]);
			ASSERT(!t[A >= B]);
			ASSERT(!t[A > B]);
			break;
		case 0:
			ASSERT(A <= B);
			ASSERT(A == B);
			ASSERT(A >= B);
			ASSERT(!t[A < B]);
			ASSERT(t[A <= B]);
			ASSERT(t[A == B]);
			ASSERT(!t[A != B]);
			ASSERT(t[A >= B]);
			ASSERT(!t[A > B]);
			break;
		case 1:
			ASSERT(A != B);
			ASSERT(A >= B);
			ASSERT(A > B);
			ASSERT(!t[A < B]);
			ASSERT(!t[A <= B]);
			ASSERT(!t[A == B]);
			ASSERT(t[A != B]);
			ASSERT(t[A >= B]);
			ASSERT(t[A > B]);
			break;
		default:
			ASSERT(0);
			break;
		}
	}
	finished();
}

