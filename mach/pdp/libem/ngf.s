.text
.globl ngf~
.globl setfloat~

ngf~:
	mov	(sp)+,r1
	jsr	pc,setfloat~
	negf	(sp)
	setl
	jmp	(r1)
