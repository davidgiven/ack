.define .rck
.sect .text
.sect .rom
.sect .data
.sect .bss

	.sect .text
.rck:
	move.l	(sp)+,a1
	move.l	(sp)+,a0	! descriptor address
	move.l	(sp),d0
	cmp.l	(a0),d0
	blt	1f
	cmp.l	2(a0),d0
	ble	2f
1:
	move.l	#ERANGE,-(sp)
	jsr	.trp
2:
	jmp	(a1)

.align 2
