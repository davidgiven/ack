.sect .text; .sect .rom; .sect .data; .sect .bss
.sect .text
.define _rmdir
_rmdir:
	jmp __rmdir
