.sect .text; .sect .rom; .sect .data; .sect .bss
.define _pause
.sect .text
_pause:
	mov	ax,29
	call	syscal
	jae	1f
	mov	(_errno),ax
	mov	ax,-1
1:
	ret
