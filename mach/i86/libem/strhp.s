.sect .text; .sect .rom; .sect .data; .sect .bss
.sect .text
.define	.strhp
.extern .reghp, .limhp, EHEAP, .trp

.strhp:
	pop	bx
	pop	ax
	mov	(.reghp),ax
	cmp	ax,(.limhp)
	jb	1f
	add	ax,02000
	and	ax,~0777
	mov	(.limhp),ax
	push	ax
	call	_brk
	or	ax,ax
	jne	2f
1:
	jmp	bx
2:
	mov	ax,EHEAP
	push	ax
	call	.trp
	jmp	bx
