.sect .text; .sect .rom; .sect .data; .sect .bss
.sect .text
.define _sync
_sync:
	jmp __sync
