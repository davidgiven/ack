.text
.globl sbf~
.globl setfloat~

sbf~:
	mov	(sp)+,r1
	jsr	pc,setfloat~
	movf	(sp)+,r0
	subf	(sp)+,r0
	negf	r0
	movf	r0,-(sp)
	setl
	jmp	(r1)
