.sect .text; .sect .rom; .sect .data; .sect .bss
.sect .text
.define _sendrec
_sendrec:
	jmp __sendrec
