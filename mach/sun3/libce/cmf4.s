.sect .text; .sect .rom; .sect .data; .sect .bss
.sect .text
.define cmf4
cmf4:
	move.l	(sp)+,a0
	move.l	(sp)+,d1
	move.l	(sp),d0
	move.l	a0,-(sp)
	jsr	(Fcmps)
	blt	1f
	bgt	2f
	clr.l	(4,sp)
	rts
1:
	move.l	#-1,(4,sp)
	rts
2:
	move.l	#1,(4,sp)
	rts

