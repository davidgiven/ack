.sect .text; .sect .rom; .sect .data; .sect .bss
.sect .text
.define fif4
fif4:
	move.l	(4,sp),d1
	move.l	(8,sp),d0
	jsr	(Fmuls)		! result in d0
	jsr	(Fstod)		! convert to double
	lea	(4,sp),a0	! build stack for modf
	move.l	a0,-(sp)
	move.l	d1,-(sp)
	move.l	d0,-(sp)
	jsr	(_modf)
	lea	(12,sp),sp
	jsr	(Fdtos)		! convert result to single
	move.l	d0,-(sp)	! save it
	move.l	(8,sp),d0	! also convert integer part to single
	move.l	(12,sp),d1
	jsr	(Fdtos)
	move.l	d0,(8,sp)	! integer part
	move.l	(sp)+,d0
	move.l	d0,(8,sp)	! fraction part
	rts
