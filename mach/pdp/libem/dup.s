.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.sect .text
.define dup~
! $Id$

dup~:
	mov	(sp)+,r3
	mov	sp,r1
	add	r0,r1
	asr	r0
1:	mov	-(r1),-(sp)
	sob	r0,1b
	jmp	(r3)
