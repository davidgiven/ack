.define _chdir
.extern _chdir
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text
_chdir:		move.w #0xC,d0
		move.l 4(sp),a0
		jmp callc
