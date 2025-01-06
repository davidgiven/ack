.sect .text; .sect .rom; .sect .data; .sect .bss
.sect .text
.define _chroot
_chroot:
	jmp __chroot
