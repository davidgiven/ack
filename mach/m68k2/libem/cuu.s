.define	.ciu
.define	.cui
.define	.cuu
.sect .text
.sect .rom
.sect .data
.sect .bss

	.sect .text
.ciu:
.cui:
.cuu:
	move.l	(sp)+,a0	! return address
	move	(sp)+,d0	! destination size
	sub	(sp)+,d0
	bgt	1f
	sub	d0,sp
	jmp	(a0)
1:
	asr	#1,d0
2:
	clr	-(sp)
	sub	#1,d0
	bgt	2b
	jmp	(a0)
