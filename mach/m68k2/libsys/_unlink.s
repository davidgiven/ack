.define __unlink
.sect .text
.sect .rom
.sect .data
.sect .bss
.extern __unlink
.sect .text
__unlink:	move.w #0xA,d0
		move.l 4(sp),a0
		jmp callc
