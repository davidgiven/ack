#include <syscall.h>

.global _geteuid
_geteuid:	
	mov	SYS_getuid, %g1
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

