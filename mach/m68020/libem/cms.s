.define	.cms
.sect .text
.sect .rom
.sect .data
.sect .bss

.sect .text
	! d0 contains set size
	! on exit d0 is zero for equal, non-zero for not equal
.cms:
	move.l	(sp)+, d2	! return address
	move.l	sp, a0		! address of top block
	lea	(sp,d0.l), a1	! address of lower block
	move.l	d0, d1
	asr.l	#2, d0
1:
	cmp.l	(a0)+, (a1)+
	bne	2f
	sub.l	#1, d0
	bne	1b
2:
	lea	(sp,d1.l*2), sp	! new sp; two blocks popped
	move.l	d2,a0
	jmp	(a0)
.align 2
