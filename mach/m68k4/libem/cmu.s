.define	.cmu
.sect .text
.sect .rom
.sect .data
.sect .bss

.sect .text
	! on entry d0: # bytes of 1 block
	! on exit d0: result
.cmu:
	move.l	(sp)+, d2	! return address
	move.l	sp, a0		! address of top block
	lea	0(sp, d0.l), a1	! address of lower block
	move.l	d0, d1
	asr.l	#2, d0
1:
	cmp.l	(a0)+, (a1)+
	bne	2f
	sub.l	#1, d0
	bne	1b		! note: on equal carry is set
2:
	bcc	3f
	neg.l	d0		! less
3:
	asl.l	#1, d1
	add.l	d1, sp		! new sp; two blocks popped
	move.l	d2, -(sp)
	rts
.align 2
