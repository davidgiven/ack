.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.sect .text
.define prf~
.extern save~,retu~,hol0,printf~
! $Header$

prf~:
	jsr	pc,save~
	mov	hol0,-(sp)
	mov	hol0+4,r0
	beq	1f
	mov	r0,r2
	mov	$050,r1
3:	movb	(r2)+,r3
	beq	2f
	cmpb	r3,$0177
	bge	1f
	cmpb	r3,$040
	blt	1f
	sob	r1,3b
	clrb	(r2)
2:	mov	sp,r1
	mov	r1,-(sp)
	mov	r0,-(sp)
	mov	$fmt,-(sp)
	jsr	pc,printf~
	add	$010,sp
	jsr	pc,printf~
	jmp	retu~
1:	mov	$name,r0
	br	2b

.sect .data
fmt:	.asciz "\"%s\", sp = %d, line %d: "
name:	.asciz "_unknown file_"
