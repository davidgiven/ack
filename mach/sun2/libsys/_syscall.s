.sect .text
.define __syscall
__syscall:
	move.l	4(sp),d0
	move.l	(sp),4(sp)
	move.l	d0,(sp)
	trap	#0
	bcs	1f
	move.l	(sp),a0
	jmp	(a0)
1:
	move.l	(sp),a0
	move.l	a0,-(sp)
	jmp	cerror
