.define __ftime
.extern __ftime
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text
__ftime:		move.w #0x23,d0
		move.l 4(sp),a0
		jmp callc
