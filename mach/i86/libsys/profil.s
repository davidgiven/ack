.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define _profil
.extern _profil, cerror
_profil:	int 0xac
	jb 9f
	ret
9:
	jmp cerror
