.define .nop
.sect .text
.sect .rom
.sect .data
.sect .bss

.sect .text
.nop:
	move.l	(.limhp),-(sp)
	move.l	(.reghp),-(sp)
	pea	(12, sp)
	move.l	(hol0),-(sp)
	pea	(fmt)
	jsr	(.diagnos)
	lea	(20, sp), sp
	rts

.sect .data
fmt:	.asciz "test %d, sp 0x%x, .reghp 0x%x, .limhp 0x%x.\n"
.align 2
