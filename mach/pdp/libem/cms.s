.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.sect .text
.define cms~
.extern save~,retu~
! $Id$

cms~:
	jsr	pc,save~
	mov	r0,r2
	add	sp,r2
	mov	r2,r4
	add	r0,r4
	asr	r0
2:	cmp	(sp)+,(r2)+
	bne	1f
	sob	r0,2b
1:	mov	r4,sp
	jmp	retu~
