.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.sect .text
.define mli~
.extern unknown~,mli4~
! $Id$

mli~:
	cmp	r0,$04
	bgt	1f
	beq	2f
	mov	(sp)+,r0
	mov	(sp)+,r1
	mul	(sp)+,r1
	mov	r1,-(sp)
	jmp	(r0)
2:	mov	(sp)+,r3
	jsr	pc,mli4~
	mov	r1,-(sp)
	mov	r0,-(sp)
	jmp	(r3)
1:	jmp	unknown~
