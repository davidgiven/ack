.define .dvu

 ! unsigned long divide
 ! register usage:
 ! 	 : d0 divisor
 !         d1 dividend
 ! exit  : d1 quotient
 !         d2 remainder
	.text
.dvu:
	move.l	(sp)+,a1	! return address
	move.l	(sp)+,d0
	move.l	(sp)+,d1
	move.l	d3,-(sp)	! save d3
	tst.l	d0
	bne	0f
	move.l	(sp)+,d3
	move.w	#EIDIVZ,-(sp)
	jsr	.trp
0:
	clr.l	d2
	move.l	#32,d3
3:
	lsl.l	#1,d1
	roxl.l	#1,d2
	cmp.l	d0,d2
	blt	4f
	sub.l	d0,d2
	add	#1,d1
4:
	sub	#1,d3
	bgt	3b
	move.l	(sp)+,d3
	jmp	(a1)
