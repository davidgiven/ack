.define __close
.extern __close
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text
__close:		move.w #0x6,d0
		move.l 4(sp),a0
		jmp callc
