.define _setuid
.sect .text
.sect .rom
.sect .data
.sect .bss
.extern _setuid
.sect .text
_setuid:	move.w #0x17,d0
		move.w 4(sp),a0
		jmp callc
