#include "syscall.h"
.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define _vfork
_vfork:
	move.l	(sp)+,a0
	pea	SYS_vfork
	trap	#0
	bcs	1f
	tst.l	d1
	bne	2f
	jmp	(a0)
1:
	move.l	d0,_errno
	move.l	#-1,d0
	jmp	(a0)
2:
	clr.l	d0
	jmp	(a0)
