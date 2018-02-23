#include "sys.h"
.define	__alarm
.extern	_errno

__alarm:
	mov	2(sp),r0
	sys	alarm
	bcc	1f
	mov	r0,_errno
	mov	$-1,r0
1:
	rts	pc
