#include <syscall.h>

.global _getegid
_getegid:	
	mov	SYS_getgid, %g1
	ta	%g0
	bgeu	0f
	sethi	%hi(cerror), %o5
	or	%o5, %lo(cerror), %o5
	jmp	%o5
	nop
0:
	mov	%o1, %o0
	retl
	nop

