.sect .text; .sect .rom; .sect .data; .sect .bss
.sect .text
.define fef8
fef8:
	move.l	(4,sp),d0
	move.l	(8,sp),d1
	lea	(8,sp),a0	! build stack for frexp
	move.l	a0,-(sp)
	move.l	d1,-(sp)
	move.l	d0,-(sp)
	jsr	(_frexp)
	lea	(12,sp),sp
	move.l	(sp),a0
	move.l	d0,(4,sp)
	move.l	(8,sp),d0
	move.l	d1,(8,sp)
	move.l	d0,(sp)
	jmp	(a0)

