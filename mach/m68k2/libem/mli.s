.define .mli


	.text
.mli:
	move.l	(sp)+,a0
	move.l	(sp)+,d1
	move.l	(sp)+,d0
	move.l	d5,-(sp)
	clr	d5
	tst.l	d0
	bpl	1f
	neg.l	d0
	not	d5
1:
	tst.l	d1
	bpl	2f
	neg.l	d1
	not	d5
2:
	move.l	d0,-(sp)
	move.l	d1,-(sp)
	jsr	.mlu
	tst	d5
	beq	3f
	neg.l	d1
	negx.l	d0
3:
	move.l	(sp)+,d5
	jmp	(a0)
