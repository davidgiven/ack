.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.sect .text
.define mlf~
.extern setfloat~
! $Id$

mlf~:
	mov	(sp)+,r1
	jsr	pc,setfloat~
	movf	(sp)+,r0
	mulf	(sp)+,r0
	movf	r0,-(sp)
	setd
	jmp	(r1)
