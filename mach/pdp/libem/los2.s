.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.sect .text
.define los2~
! $Id$

los2~:
	mov	(sp)+,r3
	cmp	r0,$01
	bne	1f
	clr	-(sp)
	bisb	(r1),(sp)
	jmp	(r3)
1:	add	r0,r1
	asr	r0
2:	mov	-(r1),-(sp)
	sob	r0,2b
	jmp	(r3)
