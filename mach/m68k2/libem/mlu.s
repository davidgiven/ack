.define .mlu
.sect .text
.sect .rom
.sect .data
.sect .bss

 ! entry : d0 multiplicand
 !         d1 multiplier
 ! exit  : d0 high order result
 !         d1 low order result

	.sect .text
.mlu:
	move.l	(sp)+,a1
	move.l	(sp)+,d1
	move.l	(sp)+,d0
	movem.l	d3/d4/d6,-(sp)
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
	movem.l	(sp)+,d3/d4/d6
	jmp	(a1)
