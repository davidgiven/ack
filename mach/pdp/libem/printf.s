.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.sect .text
.define printf~
! $Id$

printf~:
	mov	r2,-(sp)
	mov	r3,-(sp)
	mov	r4,-(sp)
	mov	sp,r3
	mov	$buff,r4
	add	$010,r3
	mov	(r3)+,r2
prloop:
	movb	(r2)+,r0
	beq	ready
	cmpb	r0,$045
	bne	1f
	movb	(r2)+,r0
	cmpb	r0,$0144
	beq	2f
	cmpb	r0,$0163
	beq	3f
1:	movb	r0,(r4)+
	br	prloop
2:	mov	(r3)+,r1
	bge	4f
	movb	$055,(r4)+
	neg	r1
4:	jsr	pc,printn
	br	prloop
printn:
	clr	r0
	div	$010,r0
	beq	5f
	mov	r1,-(sp)
	mov	r0,r1
	jsr	pc,printn
	mov	(sp)+,r1
5:	add	$060,r1
	movb	r1,(r4)+
	rts	pc
3:	mov	(r3)+,r1
7:	movb	(r1)+,r0
	bne	6f
	br	prloop
6:	movb	r0,(r4)+
	br	7b
ready:
	movb	r0,(r4)+
	sub	$buff,r4
	mov	r4,-(sp)
	mov	$buff,-(sp)
	mov	$01,-(sp)
	jsr	pc,WRITE
	add	$06,sp
	mov	(sp)+,r4
	mov	(sp)+,r3
	mov	(sp)+,r2
	rts	pc
.sect .bss
buff:	.space 256
