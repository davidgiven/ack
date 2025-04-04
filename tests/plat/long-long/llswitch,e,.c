#include "test.h"

long long a = -719560752603LL;
long long b = -319239774717LL;
long long c =  100200300401LL;
long long d =  100200300402LL;
long long e =  100200300403LL;
long long f =  100200300404LL;
long long g =  100200300405LL;
long long h =  100200300406LL;
long long i =  541934347449LL;
long long j =  727503252688LL;

int compact(long long x) {
	/* probably _csa_ */
	switch (x) {
	case 100200300401LL: return 23;
	case 100200300402LL: return 29;
	case 100200300403LL: return 31;
	case 100200300405LL: return 37;
	case 100200300406LL: return 41;
	default: return 43;
	}
}

int sparse(long long x) {
	/* probably _csb_ */
	switch (x) {
	case -719560752603LL: return 47;
	case -319239774717LL: return 53;
	case  100200300403LL: return 59;
	case  541934347449LL: return 61;
	case  727503252688LL: return 67;
	default: return 71;
	}
}

void _m_a_i_n(void) {
	ASSERT(compact(a) == 43);
	ASSERT(compact(b) == 43);
	ASSERT(compact(c) == 23);
	ASSERT(compact(d) == 29);
	ASSERT(compact(e) == 31);
	ASSERT(compact(f) == 43);
	ASSERT(compact(g) == 37);
	ASSERT(compact(h) == 41);
	ASSERT(compact(i) == 43);
	ASSERT(compact(j) == 43);

	ASSERT(sparse(a) == 47);
	ASSERT(sparse(b) == 53);
	ASSERT(sparse(c) == 71);
	ASSERT(sparse(d) == 71);
	ASSERT(sparse(e) == 59);
	ASSERT(sparse(f) == 71);
	ASSERT(sparse(g) == 71);
	ASSERT(sparse(h) == 71);
	ASSERT(sparse(i) == 61);
	ASSERT(sparse(j) == 67);

	finished();
}
