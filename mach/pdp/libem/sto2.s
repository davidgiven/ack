.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.sect .text
.define sto2~
! $Id$

sto2~:
	mov	(sp)+,r3
	cmp	r0,$01
	bne	1f
	movb	(sp),(r1)
	tst	(sp)+
	jmp	(r3)
1:	asr	r0
2:	mov	(sp)+,(r1)+
	sob	r0,2b
	jmp	(r3)
