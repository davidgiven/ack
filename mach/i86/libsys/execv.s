.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define _execv
.extern _execv, _environ, cerror
_execv:
	mov	bx,sp
	push	(_environ)
	push	4(bx)
	push	2(bx)
	push	ax
	int	0xbb
	add	sp,8
	jmp	cerror
