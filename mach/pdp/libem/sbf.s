.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.sect .text
.define sbf~
.extern setfloat~
! $Id$

sbf~:
	mov	(sp)+,r1
	jsr	pc,setfloat~
	movf	(sp)+,r0
	subf	(sp)+,r0
	negf	r0
	movf	r0,-(sp)
	setd
	jmp	(r1)
