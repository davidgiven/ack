.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define _ftime
.extern _ftime, cerror
_ftime:	int 0xa3
	jb 9f
	ret
9:
	jmp cerror
