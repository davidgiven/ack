.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.sect .text
.define ror~
.extern save~,retu~
! $Id$

ror~:
	asr	r0
	jsr	pc,save~
	mov	(sp)+,r3
3:	mov	sp,r1
	mov	r0,-(sp)
	clc
1:	ror	(r1)+
	sob	r0,1b
	bcc	2f
	bis	$0100000,02(sp)
2:	mov	(sp)+,r0
	sob	r3,3b
	jmp	retu~
