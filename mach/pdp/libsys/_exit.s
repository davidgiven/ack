#include "sys.h"
.globl	__exit

__exit:
	mov	2(sp),r0
	sys	exit
