.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.sect .text
.define set~
.extern save~,retu~,trp~
! $Id$

ESET	= 2

set~:
	jsr	pc,save~
	mov	r0,r2
	asr	r0
1:	clr	-(sp)
	sob	r0,1b
	div	$010,r0
	cmp	r0,r2
	blo	2f
	mov	$ESET,-(sp)
	jsr	pc,trp~
	jmp	retu~
2:	add	sp,r0
	bisb	bits(r1),(r0)
	jmp	retu~

.sect .data
bits:	 .data1	1,2,4,010,020,040,0100,0200
