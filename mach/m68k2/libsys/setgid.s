.define _setgid
.sect .text
.sect .rom
.sect .data
.sect .bss
.extern _setgid
.sect .text
_setgid:	move.w #0x2E,d0
		move.w 4(sp),a0
		jmp callc
