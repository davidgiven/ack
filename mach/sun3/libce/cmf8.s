.sect .text; .sect .rom; .sect .data; .sect .bss
.sect .text
.define cmf8
cmf8:
	move.l	(sp)+,a1
	lea	(8,sp),a0
	move.l	(a0),d0
	move.l	(4,a0),d1
	move.l	(sp)+,(a0)
	move.l	(sp)+,(4,a0)
	move.l	a1,-(sp)
	jsr	(Fcmpd)
	blt	1f
	bgt	2f
	clr.l	(8,sp)
3:
	move.l	(sp)+,a1
	tst.l	(sp)+
	jmp	(a1)
1:
	move.l	#-1,(8,sp)
	bra	3b
2:
	move.l	#1,(8,sp)
	bra	3b

