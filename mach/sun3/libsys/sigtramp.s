.sect .text
.define __sigtramp
__sigtramp:
	movem.l	d0/d1/a0/a1,-(sp)
	move.l	(_sigfunc),a0
	move.l	(0x14,sp),d0
	lsl.l	#2,d0
	move.l	(a0,d0),a0
	move.l	(0x1c,sp),-(sp)
	move.l	(0x1c,sp),-(sp)
	move.l	(0x1c,sp),-(sp)
	jsr	(a0)
	add.l	#12,sp
	movem.l	(sp)+,d0/d1/a0/a1
	add.l	#8,sp
	pea	(0x8b)
	trap	#0
