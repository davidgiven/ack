.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.sect .text
.define cif~,cuf~
.extern setint~,setfloat~
! $Id$

cif~:
	mov	(sp)+,r1
	mov	(sp)+,r0
	jsr	pc,setfloat~
	mov	(sp)+,r0
1:	jsr	pc,setint~
	movif	(sp)+,r0
	movf	r0,-(sp)
	setd;seti
	jmp	(r1)
cuf~:
	mov	(sp)+,r1
	mov	(sp)+,r0
	jsr	pc,setfloat~
	mov	(sp)+,r0
	cmp	r0,$02
	bne	1b
	clr	-(sp)
	mov	$04,r0
	br	1b
