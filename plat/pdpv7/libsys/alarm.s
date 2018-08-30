#include "sys.h"
.define	_alarm
.extern	_errno

_alarm:
	mov	2(sp),r0
	sys	alarm
	bcc	1f
	mov	r0,_errno
	mov	$-1,r0
1:
	rts	pc
