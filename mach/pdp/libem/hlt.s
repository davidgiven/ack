/ $Header$
.text
.globl hlt~

exit	= 1

hlt~:
	mov	(sp)+,r0
	bne	1f
	sys	exit
1:	4
