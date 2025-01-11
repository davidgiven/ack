.sect .text; .sect .rom; .sect .data; .sect .bss
.sect .text
.define _ioctl
_ioctl:
	jmp __ioctl
