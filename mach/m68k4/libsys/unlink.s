.define _unlink
.extern _unlink
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text
_unlink:	move.w #0xA,d0
		move.l 4(sp),a0
		jmp callc
