/ $Header$
.text
.globl sli~
.globl save1~,unknown~

sli~:
	mov	(sp)+,save1~
	cmp	r0,$02
	bgt	1f
	mov	(sp)+,r1
	mov	(sp)+,r0
	ash	r1,r0
	mov	r0,-(sp)
	jmp	*save1~
1:	cmp	r0,$04
	bgt	2f
	mov	02(sp),r0
	mov	04(sp),r1
	ashc	(sp)+,r0
	mov	r0,(sp)
	mov	r1,02(sp)
	jmp	*save1~
2:	jmp	unknown~
