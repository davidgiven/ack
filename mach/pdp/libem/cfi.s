.text
.globl cfi~
.globl setfloat~,setint~

cfi~:
	mov	(sp)+,r1
	mov	(sp)+,r0
	jsr	pc,setint~
	mov	(sp)+,r0
	jsr	pc,setfloat~
	movf	(sp)+,r0
	movfi	r0,-(sp)
	setl;seti
	jmp	(r1)
