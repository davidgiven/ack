.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.sect .text
.define cff~
.extern setfloat~
! $Id$

cff~:
	mov	(sp)+,r1
	mov	(sp)+,r0
	cmp	(sp)+,r0
	beq	1f
	jsr	pc,setfloat~
	movof	(sp)+,r0
	movf	r0,-(sp)
	setd
1:	jmp	(r1)
