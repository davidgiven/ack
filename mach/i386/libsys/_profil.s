.sect .text; .sect .rom; .sect .data; .sect .bss
.define __profil
.sect .text
__profil:
	mov	ax,44
	jmp	syscal
