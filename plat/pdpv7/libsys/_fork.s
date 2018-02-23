#include "sys.h"
.define	__fork
.extern	_errno

__fork:
	sys	fork
	br	1f
	bcc	2f
	mov	r0,_errno
	mov	$-1,r0
2:
	rts	pc
1:
	clr	r0
	rts	pc
