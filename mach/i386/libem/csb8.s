.sect .text; .sect .rom; .sect .data; .sect .bss
.define .csb8

.sect .text
.csb8:
		! ebx, descriptor address
		! edx:eax, index

	push	esi
	push	edi
	push	(ebx)		! default
	mov	ecx,4(ebx)	! entry count (assuming < 2**32)
1:
	add	ebx,12
	dec	ecx
	jl	4f
	mov	esi,0(ebx)
	mov	edi,4(ebx)	! descriptor's index
	sub	esi,eax
	sbb	edi,edx
	or	esi,edi
	jne	1b
	pop	edx		! drop default
	mov	ebx,8(ebx)
2:
	pop	edi
	pop	esi
	test	ebx,ebx
	jnz	3f
.extern ECASE
.extern .fat
	mov	eax,ECASE
	push	eax
	jmp	.fat
3:
	jmp	ebx
4:
	pop	ebx		! default
	jmp	2b
