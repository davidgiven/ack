.define _link
.extern _link
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text
_link:		move.l #0x9,d0
		move.l 4(sp),a0
		move.l 8(sp),d1
		jmp callc
