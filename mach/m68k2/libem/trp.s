.define	.trp
.sect .text
.sect .rom
.sect .data
.sect .bss

	.sect .text
.trp:
	move.l	(sp)+,a1	! return address
	move.w	(sp)+,d0	! error number
	move.l	a1,-(sp)
	move.w	d0,-(sp)
	cmp	#16,d0
	bcc	1f
	cmp	#8,d0
	bcc	4f
	btst	d0,.trpim
	bra	5f
4:
	btst	d0,.trpim+1
5:
	bne	3f
1:
	move.l	.trppc,a0
	move.l	a0,d0
	beq	9f
	clr.l	.trppc
	jsr	(a0)
3:
	add	#2,sp
	rts
9:
	pea	fmt
	jsr	.diagnos
	jmp	EXIT

	.sect .data
.rettrp: .data4 0
fmt:	.asciz "trap %d called\n"
.align 2
