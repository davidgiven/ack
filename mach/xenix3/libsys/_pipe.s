.sect .text; .sect .rom; .sect .data; .sect .bss
.define __pipe
.sect .text
__pipe:
	mov	ax,42
	call	syscal
	mov	dx,bx
	jae	1f
	mov	(_errno),ax
	mov	ax,-1
	cwd
1:
	ret
