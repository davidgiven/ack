/ $Header$
.text
.globl adf~
.globl setfloat~

/size in r0
adf~:
	mov	(sp)+,r1
	jsr	pc,setfloat~
	movf	(sp)+,r0
	addf	(sp)+,r0
	movf	r0,-(sp)
	setd
	jmp	(r1)
