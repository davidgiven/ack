/ $Header$
.text
.globl trp~,fat~
.globl trppc~,trpim~
	write=4.

fat~:
	jsr     pc,trp~
	4

trp~:
	mov     r0,-(sp)
	mov     04(sp),r0
	mov     02(sp),04(sp)
	mov     (sp),02(sp)
	mov     r1,(sp)
	cmp     r0,$16.
	jhis    0f
	mov     $01,r1
	ashc    r0,r1
	bit     r1,trpim~
	bne     8f
0:      mov     r2,-(sp)
	mov     r3,-(sp)
	mov     r4,-(sp)
	movf    r0,-(sp)
	movf    r1,-(sp)
	movf    r2,-(sp)
	movf    r3,-(sp)
	stfps   -(sp)
	mov     r0,-(sp)
	mov     trppc~,r0
	beq     9f
	clr     trppc~
	jsr     pc,(r0)
	tst     (sp)+
	ldfps   (sp)+
	movf    (sp)+,r3
	movf    (sp)+,r2
	movf    (sp)+,r1
	movf    (sp)+,r0
	mov     (sp)+,r4
	mov     (sp)+,r3
	mov     (sp)+,r2
8:      mov     (sp)+,r1
	mov     (sp)+,r0
	rts     pc
9:      mov	(sp)+,r0
	mov	$buf+11,r1
	mov	$4,r2
1:	mov	r0,r3
	bic	$177770,r3
	bisb	r3,-(r1)
	ash	$-3,r0
	sob	r2,1b
	mov	$2,r0
	sys	write;buf;11.
	4

.data
buf:	<err 00000\n>
