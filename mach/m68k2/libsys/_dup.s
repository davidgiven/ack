.define __dup
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text
.extern __dup
.sect .text
__dup:		move.w #0x29,d0
		move.w 4(sp),a0
		jmp call
