.define _ioctl
.extern _ioctl
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text
_ioctl:		move.l #0x36,d0
		jmp (call)
