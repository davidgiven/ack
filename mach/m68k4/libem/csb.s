.define .csb
.sect .text
.sect .rom
.sect .data
.sect .bss

	.sect .text
.csb:
	move.l	(sp)+,a0	! case descriptor
	move.l	(sp)+,d0	! index
	move.l	(a0)+,a1	! default jump address
	move.l	(a0)+,d1	! # entries
	beq	2f
	sub.l	#1,d1
1:
	cmp.l	(a0)+,d0
	beq	3f
	tst.l	(a0)+		! skip jump address
	dbf	d1,1b
2:
	move.l	a1,d1		! default jump address
	bne	4f
	move.l	#ECASE,-(sp)
	jmp	.fatal
3:
	move.l	(a0)+,a1	! get jump address
4:
	move.l	a1,-(sp)
	rts
.align 2
