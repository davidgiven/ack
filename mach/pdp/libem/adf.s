.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.sect .text
.define adf~
.extern setfloat~
! $Id$

!size in r0
adf~:
	mov	(sp)+,r1
	jsr	pc,setfloat~
	movf	(sp)+,r0
	addf	(sp)+,r0
	movf	r0,-(sp)
	setd
	jmp	(r1)
