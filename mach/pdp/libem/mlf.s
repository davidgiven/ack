.text
.globl mlf~
.globl setfloat~

mlf~:
	mov	(sp)+,r1
	jsr	pc,setfloat~
	movf	(sp)+,r0
	mulf	(sp)+,r0
	movf	r0,-(sp)
	setl
	jmp	(r1)
