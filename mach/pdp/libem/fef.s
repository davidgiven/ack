/ $Header$
.text
.globl fef~
.globl setfloat~

fef~:
	mov	(sp)+,r1
	jsr	pc,setfloat~
	movf	(sp),r0
	movei	r0,-(sp)
	movie	$0,r0
	movf	r0,02(sp)
	setd
	jmp	(r1)
