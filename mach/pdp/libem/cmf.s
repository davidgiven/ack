.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.sect .text
.define cmf~
.extern setfloat~
! $Id$

cmf~:
	jsr	pc,setfloat~
	mov	(sp)+,r1
	movf	(sp)+,r0
	movf	(sp)+,r1
	clr	r0
	cmpf	r0,r1
	setd
	cfcc
	beq	1f
	blt	2f
	dec	r0
	jmp	(r1)
2:	inc	r0
1:	jmp	(r1)
