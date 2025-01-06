.sect .text; .sect .rom; .sect .data; .sect .bss
.sect .text
.define _signal
_signal:
	jmp __signal
