.sect .text; .sect .rom; .sect .data; .sect .bss
.sect .text
.define	.strhp
.extern .reghp, .limhp, EHEAP, .trp

.strhp:
	pop	ebx
	pop	eax
	mov	(.reghp),eax
	cmp	eax,(.limhp)
	jb	1f
	add	eax,02000
	and	eax,~0777
	push	ebx
	push	eax
	call	__brk
	pop	ecx
	pop	ebx
	cmp	eax,-1
	je	2f
1:
	mov	(.limhp),ecx
	jmp	ebx
2:
	mov	eax,EHEAP
	push	ebx
	jmp	.trp
