.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define __alarm
.extern __alarm, cerror
__alarm:	int 0x9b
	jb 9f
	ret
9:
	jmp cerror
