#include "syscall.h"
.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define _wait
_wait:	
	pea	SYS_wait
	trap	#0
	bcs	1f
	tst.l	4(sp)
	beq	2f
	move.l	4(sp),a0
	move.l	d1,(a0)
2:
	rts
1:
	jmp	cerror
