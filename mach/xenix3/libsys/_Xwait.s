.sect .text; .sect .rom; .sect .data; .sect .bss
.define __Xwait
.sect .text
__Xwait:
	mov	ax,7
	call	syscal
	mov	dx,bx
	jae	1f
	mov	(_errno),ax
	mov	ax,-1
	cwd
1:
	ret
