.sect .text; .sect .rom; .sect .data; .sect .bss
.sect .text
.define dvf4
dvf4:
	move.l	(sp)+,a0
	move.l	(sp)+,d1
	move.l	(sp),d0
	move.l	a0,-(sp)
	jsr	(Fdivs)
	move.l	d0,(4,sp)
	rts
