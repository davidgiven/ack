.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.sect .text
.define strhp~
.extern trp~,reghp~
! $Header$

EHEAP	= 021

strhp~:
	mov	(sp)+,r0
	mov	(sp)+,r1
	mov	r0,-(sp)
	mov	r1,reghp~
	cmp	r1,2f+2
	blos	1f
	add	$01777,r1
	bic	$01777,r1
	mov	r1,-(sp)
	jsr	__brk
	tst	(sp)+
	tst	r0
	blt	3f
1:	rts	pc
3:	mov	$EHEAP,-(sp)
	jsr	pc,trp~
	rts	pc
