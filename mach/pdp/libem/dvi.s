.text
.globl dvi~
.globl save1~,unknown~,dvi4~

dvi~:
	mov	(sp)+,save1~
	cmp	r0,$04
	bgt	1f
	beq	2f
	mov	02(sp),r1
	sxt	r0
	div	(sp)+,r0
	mov	r0,(sp)
	br	3f
2:	jsr	pc,dvi4~
	mov	r1,-(sp)
	mov	r0,-(sp)
3:	jmp	*save1~
1:	jmp	unknown~
