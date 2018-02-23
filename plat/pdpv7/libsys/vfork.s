#include "sys.h"
.define	_vfork
.extern	_errno

_vfork:
	mov	(sp)+,r1
	sys	vfork
	br	1f
	bcc	2f
3:	mov	r0,_errno
	mov	$-1,r0
	jmp	(r1)

1:			! child, or vfork systemcall does not exist
	bcs	3b
	clr	r0
2:
	jmp	(r1)
