.define	.trp
.sect .text
.sect .rom
.sect .data
.sect .bss

	.sect .text
.trp:
	cmp.l	#16,4(sp)
	bcc	1f
	move.l	d0,-(sp)
	move.l	8(sp),d0
	btst	d0,.trpim
	bne	3f
1:
	move.l	a0,-(sp)
	move.l	.trppc,a0
	move.l	a0,d0
	beq	9f
	clr.l	.trppc
	move.l	12(sp),-(sp)
	jsr	(a0)
	add.l	#4,sp
	move.l	(sp)+,a0
3:
	move.l	(sp)+,d0
	move.l	(sp)+,(sp)
	rts
9:
	move.l	(sp)+,a0
	move.l	(sp)+,d0
	move.l	4(sp),-(sp)
	pea	fmt
	jsr	.diagnos
	add	#4,sp
	jsr	__cleanup
	jmp	EXIT

	.sect .data
fmt:	.asciz "trap %d called\n"
.align 2
