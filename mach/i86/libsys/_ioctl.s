.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define __ioctl
.extern __ioctl, cerror
__ioctl:	int 0xb6
	jb 9f
	ret
9: jmp cerror
