/ $Header$
.text
.globl strhp~
.globl fat~,reghp~,_end
indir	= 0

break	= 17.
EHEAP	= 17.

strhp~:
	mov	(sp)+,r0
	mov	(sp)+,r1
	mov	r1,reghp~
	cmp	r1,2f+2
	blos	1f
	add	$01777,r1
	bic	$01777,r1
	mov	r1,2f+2
	sys	indir ; 2f
	bcs	3f
1:	jmp	(r0)
3:	mov	$EHEAP,-(sp)
	jmp	fat~
.data
2:	sys	break; _end
