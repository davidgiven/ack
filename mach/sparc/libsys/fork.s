#include "SYS.h"

LABEL_(fork); BODY1(fork)
	orcc	%g0, %o1, %g0
	bne,a	1f
	clr	%o0
1:
	retl
	nop
