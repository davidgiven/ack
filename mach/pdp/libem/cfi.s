.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.sect .text
.define cfi~
.extern setfloat~,setint~
! $Id$

cfi~:
	mov	(sp)+,r1
	mov	(sp)+,r0
	jsr	pc,setint~
	mov	(sp)+,r0
	jsr	pc,setfloat~
	movf	(sp)+,r0
	movfi	r0,-(sp)
	setd;seti
	jmp	(r1)
