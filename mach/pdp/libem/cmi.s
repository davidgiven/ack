.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.sect .text
.define cmi~
.extern cmi4~,unknown~
! $Id$

! Size in r0
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
