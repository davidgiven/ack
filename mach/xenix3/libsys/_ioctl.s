.sect .text; .sect .rom; .sect .data; .sect .bss
.define __ioctl
.sect .text
__ioctl:
	mov	ax,54
	jmp	sysal
