/ $Header$
.text
.globl prf~
.globl save~,retu~,hol0,_printf

prf~:
	jsr	pc,save~
	mov	hol0,-(sp)
	mov	hol0+4,r0
	beq	1f
	mov	r0,r2
	mov	$40.,r1
3:	movb	(r2)+,r3
	beq	2f
	cmpb	r3,$0177
	bge	1f
	cmpb	r3,$040
	blt	1f
	sob	r1,3b
	clrb	(r2)
2:	mov	sp,r1
	mov	r1,-(sp)
	mov	r0,-(sp)
	mov	$fmt,-(sp)
	jsr	pc,_printf
	add	$010,sp
	jsr	pc,_printf
	jmp	retu~
1:	mov	$name,r0
	br	2b

.data
fmt:	<"%s", sp = %d, line %d: \0>
name:	<_unknown file_\0>
