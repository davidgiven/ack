.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.sect .text
.define cmu~
.extern unknown~,cmu4~
! $Id$

cmu~:
	cmp	r0,$02
	bne	3f
	mov	(sp)+,r1
	clr	r0
	cmp	(sp)+,(sp)+
	beq	2f
	bhi	1f
	inc	r0
	br	2f
1:
	dec	r0
2:
	jmp	(r1)
3:	cmp	r0,$04
	bne	2f
	jmp	cmu4~
2:	jmp	unknown~
