.define .rck

	.text
.rck:
	movem.l	d0/a0,.savreg
	move.l	(sp)+,.savret
	move.l	(sp)+,a0	! descriptor address
	move.l	(sp),d0
	cmp.l	(a0),d0
	blt	1f
	cmp.l	2(a0),d0
	ble	2f
1:
	move.l	#ERANGE,-(sp)
	jsr	.trp
2:
	movem.l	.savreg,d0/a0
	move.l	.savret,-(sp)
	rts
.align 2
