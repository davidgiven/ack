.define	.sig

	.text
.sig:
	movem.l	a1/a2,.savreg
	move.l	(sp)+,.savret
	move.l	(sp)+,a1	! trap pc
	move.l	.trppc,-(sp)
	move.l	a1,.trppc
	movem.l	.savreg,a1/a2
	move.l	.savret,-(sp)
	rts
.align 2
