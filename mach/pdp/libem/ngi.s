/ $Header$
.text
.globl ngi~
.globl unknown~

ngi~:
	mov	(sp)+,r1
	cmp	r0,$02
	bgt	1f
	neg	(sp)
	jmp	(r1)
1:	cmp	r0,$04
	bgt	2f
	neg	(sp)
	neg	02(sp)
	sbc	(sp)
	jmp	(r1)
2:	jmp	unknown~
