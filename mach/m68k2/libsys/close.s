.define _close
.sect .text
.sect .rom
.sect .data
.sect .bss
.extern _close
.sect .text
_close:		move.w #0x6,d0
		move.w 4(sp),a0
		jmp callc
