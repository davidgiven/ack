.sect .text; .sect .rom; .sect .data; .sect .bss
.sect .text
.define _umount
_umount:
	jmp __umount
