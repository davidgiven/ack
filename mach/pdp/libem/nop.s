/ $Header$
.text
.globl nop~
.globl hol0,prf~

nop~:
	mov	hol0,-(sp)
	mov	$fmt,-(sp)
	jsr	pc,prf~
	add	$04,sp
	rts	pc
.data
fmt:	<test %d\n\0>
