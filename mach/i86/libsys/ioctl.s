.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define _ioctl
.extern _ioctl, cerror
_ioctl:	int 0xb6
	jb 9f
	ret
9: jmp cerror
