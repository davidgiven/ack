.define .mlu

 ! entry : d0 multiplicand
 !         d1 multiplier
 ! exit  : d0 high order result
 !         d1 low order result

	.text
.mlu:
	move.l	(sp)+,.savret
	move.l	(sp)+,d1
	move.l	(sp)+,d0
	cmp.l	#32768,d0
	bgt	1f
	cmp.l	#32768,d1
	bls	2f
1:	movem.l	d2/d3/d4/d6,.savreg
	move.l	d1,d3
	move.l	d1,d2
	swap	d2
	move.l	d2,d4
	mulu	d0,d1
	mulu	d0,d2
	swap	d0
	mulu	d0,d3
	mulu	d4,d0
	clr.l	d6
	swap	d1
	add	d2,d1
	addx.l	d6,d0
	add	d3,d1
	addx.l	d6,d0
	swap	d1
	clr	d2
	clr	d3
	swap	d2
	swap	d3
	add.l	d2,d0
	add.l	d3,d0
	movem.l	.savreg,d2/d3/d4/d6
	move.l	.savret,-(sp)
	rts
2:	mulu	d0,d1
	clr	d0
	move.l	.savret,-(sp)
	rts
.align 2
