.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.sect .text
.define dvf~
.extern setfloat~
! $Id$

dvf~:
	mov	(sp)+,r1
	jsr	pc,setfloat~
	movf	(sp)+,r0
	movf	(sp)+,r1
	divf	r0,r1
	movf	r1,-(sp)
	setd
	jmp	(r1)
