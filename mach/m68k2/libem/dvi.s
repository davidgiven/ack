.define .dvi
.sect .text
.sect .rom
.sect .data
.sect .bss

 ! signed long divide
	.sect .text
.dvi:
	move.l	(sp)+,a0	! return address
	move.l	(sp)+,d0
	move.l	(sp)+,d1
	move.l	d3,-(sp)	! save d3 and d4
	move.l	d4,-(sp)
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
	neg.l	d2	! remainder
6:
	move.l	(sp)+,d4	! restore d4 and d3
	move.l	(sp)+,d3
	jmp	(a0)
