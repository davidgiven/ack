.define .sts
.sect .text
.sect .rom
.sect .data
.sect .bss

	! d0 : # bytes
	! a0 : destination address
	! a1 : return address
.sect .text
.sts:
	move.l	(sp)+, a1
	move.l	(sp)+, d0
	move.l	(sp)+, a0
	cmp.l	#1, d0
	bne	1f
	move.l	(sp)+, d0
	move.b	d0, (a0)
	bra	4f

1:	cmp.l	#2, d0
	bne	2f
	move.l	(sp)+, d0
	move.w	d0, (a0)
	bra	4f
2:
	asr.l	#2, d0
3:
	move.l	(sp)+, (a0)+
	sub.l	#1, d0
	bne	3b
4:
	jmp	(a1)		! return
.align 2
