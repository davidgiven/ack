.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.sect .text
.define strhp~
.extern trp~,reghp~,_end
! $Header$
indir	= 0

break	= 021
EHEAP	= 021

strhp~:
	mov	(sp)+,r0
	mov	(sp)+,r1
	mov	r1,reghp~
	cmp	r1,2f+2
	blos	1f
	add	$01777,r1
	bic	$01777,r1
	mov	r1,2f+2
	sys	indir ; .data2 2f
	bcs	3f
1:	jmp	(r0)
3:	mov	$EHEAP,-(sp)
	jsr	pc,trp~
	jmp	(r0)
.sect .data
2:	sys	break; .data2 _end
