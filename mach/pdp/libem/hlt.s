.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.sect .text
.define hlt~
! $Header$

exit	= 1

hlt~:
	mov	(sp)+,r0
	bne	1f
	sys	exit
1:	.data2 4
