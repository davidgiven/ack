.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.sect .text
.define sri~
.extern unknown~
! $Id$

! Size in r0
sri~:
	mov	(sp)+,r3~
	cmp	r0,$02
	bgt	1f
	mov	(sp)+,r1
	mov	(sp)+,r0
	neg	r1
	ash	r1,r0
	mov	r0,-(sp)
	jmp	(r3)
1:	cmp	r0,$04
	bgt	2f
	mov	02(sp),r0
	mov	04(sp),r1
	neg	(sp)
	ashc	(sp)+,r0
	mov	r0,(sp)
	mov	r1,02(sp)
	jmp	(r3)
2:	jmp	unknown~
