/ $Header$
.text
.globl cmi~
.globl cmi4~,unknown~

/ Size in r0
cmi~:
	cmp	r0,$02
	bne	1f
	mov	(sp)+,r1
	mov	(sp)+,r0
	sub	(sp)+,r0
	neg	r0
	jmp	(r1)
1:	cmp	r0,$04
	bne	2f
	jmp	cmi4~
2:	jmp	unknown~
