#include "syscall.h"
.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define __pipe
__pipe:
	pea	SYS_pipe
	trap	#0
	bcs	1f
	move.l	4(sp),a0
	move.l	d0,(a0)+
	move.l	d1,(a0)
	clr.l	d0
	rts
1:
	jmp	cerror
