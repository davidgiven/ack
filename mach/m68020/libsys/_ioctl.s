.define __ioctl
.extern __ioctl
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text
__ioctl:		move.l #0x36,d0
		jmp (call)
