.define _setgid
.extern _setgid
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text
_setgid:	move.w #0x2E,d0
		move.l 4(sp),a0
		jmp callc
