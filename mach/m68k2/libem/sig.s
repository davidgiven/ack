.define	.sig

	.text
.sig:
	move.l	(sp)+,a0
	move.l	(sp)+,a1	! trap pc
	move.l	.trppc,-(sp)
	move.l	a1,.trppc
	jmp	(a0)
