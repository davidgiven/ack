/ $Header$
.text
.globl iaar~
.globl aar~,trp~

EILLINS = 18.

iaar~:
	mov	(sp)+,r0
	cmp	(sp)+,$02
	bne	1f
	mov	02(sp),r1
	mov	r0,02(sp)
	mov	(sp)+,r0
	jmp	aar~
1:	mov	$EILLINS,-(sp)
	jsr	pc,trp~
	add	$06,sp
	jmp	(r0)
