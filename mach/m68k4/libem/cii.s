.define	.cii
.sect .text
.sect .rom
.sect .data
.sect .bss

	.sect .text
.cii:
	move.l	(sp)+,a0	! return address
	move.l	(sp)+,d0	! destination size
	move.l	(sp)+,d1
	move.l	(sp),d2
	cmp.l	#1,d1
	bne	1f
	move.l	#2,d1
	ext.w	d2
1:
	cmp.l	#2,d1
	bne	1f
	move.l	#4,d1
	ext.l	d2
1:
	move.l	d2,(sp)
	sub.l	d1,d0	
	bgt	1f
	sub.l	d0,sp		! pop extra bytes
	bra	3f
1:
	clr.l	d1
	tst.l	d2
	bge	1f
	move.l	#-1,d1
1:
	asr	#2,d0
2:
	move.l	d1,-(sp)
	dbf	d0,2b
3:
	move.l	a0,-(sp)
	rts

.align 2
