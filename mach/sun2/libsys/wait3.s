#include "syscall.h"
.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define _wait3
_wait3:
	move.l	8(sp),d0
	move.l	12(sp),d1
	pea	SYS_wait
	or.b	#0x1f,ccr
	trap	#0
	bcc	1f
	jmp	cerror
1:
	tst.l	4(sp)
	beq	1f
	move.l	4(sp),a0
	move.l	d1,(a0)
1:
	rts
