.define __dup
.extern __dup
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text
__dup:		move.w #0x29,d0
		move.l 4(sp),a0
		jmp call
