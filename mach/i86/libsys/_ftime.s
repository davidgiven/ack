.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define __ftime
.extern __ftime, cerror
__ftime:	int 0xa3
	jb 9f
	ret
9:
	jmp cerror
