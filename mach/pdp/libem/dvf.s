.text
.globl dvf~
.globl setfloat~

dvf~:
	mov	(sp)+,r1
	jsr	pc,setfloat~
	movf	(sp)+,r0
	movf	(sp)+,r1
	divf	r0,r1
	movf	r1,-(sp)
	setl
	jmp	(r1)
