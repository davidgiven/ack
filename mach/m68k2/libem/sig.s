.define	.sig
.sect .text
.sect .rom
.sect .data
.sect .bss

	.sect .text
.sig:
	move.l	(sp)+,a0
	move.l	(sp)+,a1	! trap pc
	move.l	.trppc,-(sp)
	move.l	a1,.trppc
	jmp	(a0)
