.define .csb4
.sect .text
.sect .rom
.sect .data
.sect .bss

	.sect .text
.csb4:
	move.l	(sp)+,a0	! case descriptor
	move.l	(sp)+,d0	! index
	move.l	(a0)+,a1	! default jump address
	move.w	(a0)+,d1	! # entries
	beq	2f
1:
	cmp.l	(a0)+,d0
	beq	3f
	tst.l	(a0)+		! skip jump address
	sub.l	#1,d1
	bgt	1b
2:
	move.l	a1,d1		! default jump address
	bne	4f
	move.w	#ECASE,-(sp)
	jmp	.fat
3:
	move.l	(a0)+,a1	! get jump address
4:
	jmp	(a1)
