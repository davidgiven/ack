.define .dvi

 ! signed long divide
	.text
.dvi:
	movem.l	d0/d2,.savdvi
	move.l	(sp)+,.retdvi
	move.l	(sp)+,d0
	move.l	(sp)+,d1
	clr.l	d4
	tst.l	d0	! divisor
	bpl	1f
	neg.l	d0
	not	d4
1:
	tst.l	d1	! dividend
	bpl	2f
	neg.l	d1
	not	d4
	swap	d4
	not	d4
	swap	d4
2:
	move.l	d1,-(sp)
	move.l	d0,-(sp)
	jsr	.dvu
	tst	d4
	beq	5f
	neg.l	d1	! quotient
5:
	tst.l	d4
	bpl	6f
	neg.l	d3	! remainder
6:
	movem.l .savdvi,d0/d2
	move.l	.retdvi,-(sp)
	rts
.data
.savdvi:	.space 8
.retdvi:	.long 0
.text
.align 2
