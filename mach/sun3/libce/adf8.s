.sect .text; .sect .rom; .sect .data; .sect .bss
.sect .text
.define adf8
adf8:
	move.l	(sp)+,a1
	lea	(8,sp),a0
	move.l	(a0),d0
	move.l	(4,a0),d1
	move.l	(sp)+,(a0)
	move.l	(sp)+,(4,a0)
	move.l	a1,-(sp)
	jsr	(Faddd)
	move.l	d0,(4,sp)
	move.l	d1,(8,sp)
	rts
