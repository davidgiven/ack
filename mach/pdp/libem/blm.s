/ $Header$
.globl blm~
.globl save~,retu~

/ Size in r0
blm~:
	jsr	pc,save~
	mov	(sp)+,r2
	mov	(sp)+,r3
	mov	r0,r1
	asr	r0
	beq	2f
/ Now avoid wrong copy.
/ The pieces may overlap !
	cmp	r3,r2
	beq	2f
	blt	3f
1:
	mov	(r3)+,(r2)+
	sob	r0,1b
2:
	jmp	retu~
3:
	add	r1,r3
	add	r1,r2
4:
	mov	-(r3),-(r2)
	sob	r0,4b
	br	2b
