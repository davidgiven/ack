/ $Header$
.text
.globl fif~
.globl setfloat~

fif~:
	mov	(sp)+,r1
	jsr	pc,setfloat~
	movf	(sp)+,r0
	modf	(sp)+,r0
	movf	r0,-(sp)
	movf	r1,-(sp)
	setd
	jmp	(r1)
