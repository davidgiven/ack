.define	.strhp

	.text
.strhp:
	move.l	(sp)+,a0
	move.l	(sp)+,d0	! heap pointer
	move.l	d0,.reghp
	cmp.l	.limhp,d0
	bmi	1f
	add.l	#0x400,d0
	and.l	#~0x3ff,d0
	move.l	d0,.limhp
	move.l	d0,-(sp)
	jsr	_brk
	tst.l	(sp)+
	tst.w	d0
	bne	2f
1:
	jmp	(a0)
2:
	move.w	#EHEAP,-(sp)
	jmp	.fat
