.define _umask
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text
.extern _umask
.sect .text
_umask:		move.w #0x3C,d0
		move.w 4(sp),a0
		jmp call
