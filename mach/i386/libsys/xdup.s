.sect .text; .sect .rom; .sect .data; .sect .bss
.define ___xdup
.sect .text
___xdup:
	mov	ax,41
	jmp	syscal
