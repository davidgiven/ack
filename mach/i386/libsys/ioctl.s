.sect .text; .sect .rom; .sect .data; .sect .bss
.define _ioctl
.sect .text
_ioctl:
	mov	ax,54
	jmp	syscal
