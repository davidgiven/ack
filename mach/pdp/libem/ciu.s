/ $Header$
.text
.globl cuu~
cuu~:
	mov	(sp)+,r1
	mov	(sp)+,r0
	sub	(sp)+,r0
	ble	1f
	asr	r0
2:	clr	-(sp)
	sob	r0,2b
	jmp	(r1)
1:	sub	r0,sp
	jmp	(r1)
