.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.sect .text
.define ret~,lfr~
.extern unknown~, retar
! $Id$

! Size in r0
ret~:
	mov	r0,r1
	beq	1f
	asr	r1
	add	$retar,r0
	cmp	r0,$retend
	bhi	9f
3:	mov	(sp)+,-(r0)
	sob	r1,3b
1:	mov	r5,sp
	mov	(sp)+,r5
	rts	pc
9:	jmp	unknown~
lfr~:
	mov	(sp)+,r3
	asr	r0
	beq	4f
	mov	$retar,r1
5:	mov	(r1)+,-(sp)
	sob	r0,5b
4:	jmp	(r3)
