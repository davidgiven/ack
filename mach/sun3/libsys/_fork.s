#include "syscall.h"
.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define __fork
__fork:
	pea	(SYS_fork)
	trap	#0
	bcs	1f
	tst.l	d1
	beq	2f
	clr.l	d0
2:
	rts
1:
	jmp	(cerror)
