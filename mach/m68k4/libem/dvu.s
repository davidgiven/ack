.define .dvu

 ! unsigned long divide
 ! register usage:
 ! 	 : d0 divisor
 !         d1 dividend
 ! exit  : d1 quotient
 !         d2 remainder
	.text
.dvu:
	movem.l	d0/d3,.savreg
	move.l	(sp)+,.savret
	move.l	(sp)+,d0
	move.l	(sp)+,d1
	tst.l	d0
	bne	0f
	move.w	#EIDIVZ,-(sp)
	jsr	.trp
0:
	clr.l	d2
	move.l	#31,d3
3:
	lsl.l	#1,d1
	roxl.l	#1,d2
	cmp.l	d0,d2
	blt	4f
	sub.l	d0,d2
	add	#1,d1
4:
	dbf	d3,3b
	movem.l .savreg,d0/d3
	move.l	.savret,-(sp)
	rts
.align 2
