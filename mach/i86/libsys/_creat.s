.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define __creat
.extern __creat, cerror
__creat:	int 0x88
	jb 9f
	ret
9: jmp cerror
