.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define fif~
.extern setfloat~
! $Id$

fif~:
	mov	(sp)+,r1
	jsr	pc,setfloat~
	movf	(sp)+,r0
	modf	(sp)+,r0
	movf	r0,-(sp)
	movf	r1,-(sp)
	setd
	jmp	(r1)
