.define	.trp
.sect .text
.sect .rom
.sect .data
.sect .bss

	.sect .text
.trp:
	movem.l	d0/d1/a0, -(sp)
	move.l	(12, sp), a0	! return address
	move.l	(16, sp), d0	! error number
	move.l	a0, (16, sp)
	move.l	d0, (12, sp)
	move.l	d0,-(sp)
	cmp.l	#16, d0
	bcc	1f
	move.l	(.trpim), d1
	btst	d0, d1
	bne	3f
1:
	move.l	(.trppc), a0
	move.l	a0, d0
	beq	9f
	clr.l	(.trppc)
	jsr	(a0)
3:
	add.l	#4, sp
	movem.l	(sp)+, d0/d1/a0
	add.l	#4, sp
	rts
9:
	pea	(fmt)
	jsr	(.diagnos)
	lea	(4, sp), sp
	jsr	(__cleanup)
	jmp	(EXIT)

.sect .data
fmt:	.asciz "trap %d called\n"
.align 2
