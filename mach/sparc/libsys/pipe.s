#include "SYS.h"

LABEL_(pipe); ENTRY1;
	mov	%o0,%o2
	BODY1(pipe)
	st	%o0,[%o2]
	st	%o1,[%o2+4]
	retl
	clr	%o0
