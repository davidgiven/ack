.sect .text; .sect .rom; .sect .data; .sect .bss
.define __profil
.sect .text
__profil:
	mov	ax,44
	mov	bx,sp
	add	bx,2
	call	syscal
	jae	1f
	mov	(_errno),ax
	mov	ax,-1
1:
	ret
