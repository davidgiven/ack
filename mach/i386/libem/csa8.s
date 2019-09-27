.sect .text; .sect .rom; .sect .data; .sect .bss
.define .csa8

.sect .text
.csa8:
		! ebx, descriptor address
		! edx:eax, index

	mov	ecx,(ebx)	! default
	sub	eax,4(ebx)
	sbb	edx,8(ebx)	! index - lower bound
	push	edx
	push	eax
	mov	eax,12(ebx)
	mov	edx,16(ebx)	! upper bound - lower bound
	sub	eax,(esp)
	sbb	edx,4(esp)	! upper bound - index
	pop	eax
	pop	edx
	jb	1f		! jump if upper bound < index
	! assuming edx:eax < 2**30
	mov	ebx,20(ebx)(eax*4)
	test	ebx,ebx
	jnz	2f
1:
	mov	ebx,ecx
	test	ebx,ebx
	jnz	2f
.extern ECASE
.extern .fat
	mov	eax,ECASE
	push	eax
	jmp	.fat
2:
	jmp	ebx
