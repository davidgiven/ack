.text
.globl adi~
.globl unknown~

/size in r0
adi~:
	mov	(sp)+,r1
	cmp	r0,$04
	bgt	1f
	cmp	r0,$02
	bgt	2f
	add	(sp)+,(sp)
	jmp	(r1)
2:	add	(sp)+,02(sp)
	add	(sp)+,02(sp)
	adc	(sp)
	jmp	(r1)
1:
	jmp	unknown~
