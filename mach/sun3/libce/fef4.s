.sect .text; .sect .rom; .sect .data; .sect .bss
.sect .text
.define fef4
fef4:
	move.l	(4,sp),d0
	jsr	(Fstod)		! convert to double
	lea	(4,sp),a0	! build stack for frexp
	move.l	a0,-(sp)
	move.l	d1,-(sp)
	move.l	d0,-(sp)
	jsr	(_frexp)
	lea	(12,sp),sp
	jsr	(Fdtos)		! convert result to single
	move.l	(sp),a0
	move.l	(4,sp),d1
	move.l	d0,(4,sp)
	move.l	d1,(sp)
	jmp	(a0)

