.define	.strhp

	.text
.strhp:
	move.l	d0,.savreg
	move.l	(sp)+,.savret
	move.l	(sp)+,d0	! heap pointer
	move.l	d0,.reghp
	cmp.l	.limhp,d0
	blt	1f
	add.l	#0x400,d0
	and.l	#~0x3ff,d0
	move.l	d0,.limhp
	cmp.l	d0,sp
	ble	2f
1:
	move.l	.savreg,d0
	move.l	.savret,-(sp)
	rts
2:
	move.w	#EHEAP,-(sp)
	jmp	.fat
.align 2
