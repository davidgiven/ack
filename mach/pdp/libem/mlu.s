.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.sect .text
.define mlu~
.extern unknown~,mlu4~
! $Id$

mlu~:
	cmp	r0,$04
	bgt	1f
	beq	2f
	mov	(sp)+,r0
	mov	(sp)+,r1
	mul	(sp)+,r1
	mov	r1,-(sp)
	jmp	(r0)
2:	mov	(sp)+,r3
	jsr	pc,mlu4~
	mov	r1,-(sp)
	mov	r0,-(sp)
	jmp	(r3)
1:	jmp	unknown~
