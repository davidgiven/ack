.sect .text; .sect .rom; .sect .data; .sect .bss
.sect .text
.define _execn
.define _execnl
_execn:
	jmp __execn
_execnl:
	jmp __execnl
