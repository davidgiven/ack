.define .lpb
.sect .text
.sect .rom
.sect .data
.sect .bss
.extern .lpb
	.sect .text
.lpb:
	! convert local to argument base
	! should not destroy register d2 (used by lxa/lxl)

	move.l	(sp)+,a1	! return address
	move.l	(sp)+,a0	! local base
	move.w	4(a0),d0	! register save word
	move.w	d0,d1
	and.l	#7,d0		! #data registers
	and.l 	#070,d1
	asr.l	#3,d1		! #address registers
	add.w	d1,d0
	asl.l	#2,d0		! 4 * #registers
	add.w	#10,d0		! reg. save word, lb, pc
	add.l	d0,a0
	jmp	(a1)
