.define	.cii
.sect .text
.sect .rom
.sect .data
.sect .bss

	.sect .text
.cii:
	move.l	(sp)+,a0	! return address
	move	(sp)+,d0	! destination size
	sub	(sp)+,d0	! destination - source size
	bgt	1f
	sub	d0,sp		! pop extra bytes
	bra	3f
1:
	move	(sp),d1
	ext.l	d1
	swap	d1
	asr	#1,d0
2:
	move.w	d1,-(sp)
	sub	#1,d0
	bgt	2b
3:
	jmp	(a0)
