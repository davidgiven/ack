#include "SYS.h"

.global __exit
__exit:
	ld	[%l0], %o0
	set	SYS_exit, %g1
	ta	%g0
