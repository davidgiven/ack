.define	.trp

	.text
.trp:
	move.l	(sp)+,a2	! return address
	move.l	(sp)+,d0	! error number
	move.l	a2,-(sp)
	move.l	d0,-(sp)
	cmp.l	#16,d0
	bcc	1f
	btst	d0,.trpim
	bne	3f
1:
	move.l	.trppc,a0
	move.l	a0,d0
	beq	9f
	clr.l	.trppc
	jsr	(a0)
3:
	add	#4,sp
	rts
9:
	pea	fmt
	jsr	.diagnos
	add	#4,sp
	jsr	__cleanup
	jmp	EXIT

	.data
fmt:	.asciz "trap %d called\n"
.align 2
