.sect .text; .sect .rom; .sect .data; .sect .bss
.sect .text
.define cff
cff:
	move.l	(sp)+,a0
	move.l	(sp)+,d1
	move.l	(sp)+,d0
	cmp.l	d0,d1
	beq	2f
	cmp.l	#4,d0
	bne	1f
	move.l	(sp),d0
	tst.l	-(sp)
	move.l	a0,-(sp)
	jsr	(Fstod)
	move.l	d0,(4,sp)
	move.l	d1,(8,sp)
2:
	rts
1:
	move.l	(sp),d0
	move.l	(4,sp),d1
	move.l	a0,(sp)
	jsr	(Fdtos)
	move.l	d0,(4,sp)
	rts
