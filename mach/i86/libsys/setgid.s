.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define _setgid
.extern _setgid, cerror
setgid:	int 0xae
	jb 9f
	xor ax,ax
	ret
9: jmp cerror
