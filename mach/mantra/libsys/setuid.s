.define _setuid
.extern _setuid
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text
_setuid:	move.l #0x17,d0
		move.l 4(sp),a0
		jmp callc
