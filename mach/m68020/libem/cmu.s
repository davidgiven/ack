.define	.cmu
.sect .text
.sect .rom
.sect .data
.sect .bss

.sect .text
	! on entry d0: # bytes in 1 block
	! on exit d0: result
.cmu:
	move.l	(sp)+, d1	! return address
	move.l	sp, a0		! address of top block
	lea	(sp,d0.l), a1	! address of lower block
	move.l	d1,-(sp)
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
	move.l	(sp)+,a0
	lea	(sp,d1.l*2), sp	! new sp; two blocks popped
	jmp	(a0)
.align 2
