.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.sect .text
.define rmi~
.extern unknown~,rmi4~
! $Id$

rmi~:
	mov	(sp)+,r3
	cmp	r0,$04
	bgt	1f
	beq	2f
	mov	02(sp),r1
	sxt	r0
	div	(sp)+,r0
	mov	r1,(sp)
	br	3f
2:	jsr	pc,rmi4~
	mov	r1,-(sp)
	mov	r0,-(sp)
3:	jmp	(r3)
1:	jmp	unknown~
