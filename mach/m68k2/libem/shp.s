.define	.strhp
.sect .text
.sect .rom
.sect .data
.sect .bss

	.sect .text
.strhp:
	move.l	(sp)+,a0
	move.l	(sp)+,d0	! heap pointer
	move.l	d0,.reghp
	cmp.l	.limhp,d0
	bmi	1f
	add.l	#0x400,d0
	and.l	#~0x3ff,d0
	move.l	a0,-(sp)
	move.l	d0,-(sp)
	move.l	d0,.limhp
	jsr	_brk
	tst.l	(sp)+
	move.l	(sp)+,a0
	tst.w	d0
	bne	2f
1:
	jmp	(a0)
2:
	move.w	#EHEAP,-(sp)
	jsr	.trp
	jmp	(a0)
