#include "SYS.h"

LABEL__(pipe); ENTRY1
LABEL(pipe)
	mov	%o0,%o2
	BODY1(pipe)
	st	%o0,[%o2]
	st	%o1,[%o2+4]
	retl
	clr	%o0
