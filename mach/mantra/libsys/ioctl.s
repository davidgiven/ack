.define _ioctl
.extern _ioctl
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text
_ioctl:		move.l #0x36,d0
		move.l 4(sp),a0
		move.l 8(sp),d1
		move.l 12(sp),a1
		jmp callc
