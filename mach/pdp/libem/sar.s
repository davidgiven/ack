/ $Header$
.text
.globl sar~

sar~:
	mov	(sp)+,r3
	sub	(r0),r1
	mov	04(r0),r0
	mul	r0,r1
	add	(sp)+,r1
	asr	r0
	beq	1f
2:	mov	(sp)+,(r1)+
	sob	r0,2b
	jmp	(r3)
1:	movb	(sp)+,(r1)
	jmp	(r3)
