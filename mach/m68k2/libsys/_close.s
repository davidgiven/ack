.define __close
.sect .text
.sect .rom
.sect .data
.sect .bss
.extern __close
.sect .text
__close:		move.w #0x6,d0
		move.w 4(sp),a0
		jmp callc
