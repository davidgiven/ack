.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define _creat
.extern _creat, cerror
_creat:	int 0x88
	jb 9f
	ret
9: jmp cerror
