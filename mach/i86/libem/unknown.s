.sect .text; .sect .rom; .sect .data; .sect .bss
.sect .text
.define .unknown
.extern EILLINS, .fat

.unknown:
	mov  ax,EILLINS
	push ax
	jmp  .fat
