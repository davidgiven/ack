.sect .text; .sect .rom; .sect .data; .sect .bss
.sect .text
.define adf4
adf4:
	move.l	(sp)+,a0
	move.l	(sp)+,d1
	move.l	(sp),d0
	move.l	a0,-(sp)
	jsr	(Fadds)
	move.l	d0,(4,sp)
	rts
