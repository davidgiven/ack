#include "syscall.h"
.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define __sbrk,__brk
__sbrk:
	move.l	(4,sp),d0
	add.l	#3,d0
	move.l	#~3,d1
	and.l	d1,d0
	move.l	d0,a0
	move.l	(curbrk),d0
	add.l	#3,d0
	and.l	d1,d0
	move.l	d0,(curbrk)
	add.l	d0,a0
	move.l	a0,(4,sp)
	pea	(17)	! not SYS_sbrk
	trap	#0
	bcs	1f
	move.l	(curbrk),d0
	move.l	(4,sp),(curbrk)
	rts
1:
	jmp	(cerror)

__brk:
	move.l	(4,sp),d0
	add.l	#3,d0
	move.l	#~3,d1
	and.l	d1,d0
	move.l	d0,(4,sp)
	pea	(17)
	trap	#0
	bcs	1f
	move.l	(4,sp),(curbrk)
	clr.l	d0
	rts
1:
	jmp	(cerror)

.sect .data
curbrk:	.data4 endbss
