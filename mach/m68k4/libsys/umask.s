.define _umask
.extern _umask
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text
_umask:		move.w #0x3C,d0
		move.l 4(sp),a0
		jmp call
