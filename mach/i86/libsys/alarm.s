.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define _alarm
.extern _alarm, cerror
_alarm:	int 0x9b
	jb 9f
	ret
9:
	jmp cerror
