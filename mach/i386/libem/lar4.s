.sect .text; .sect .rom; .sect .data; .sect .bss
.sect .text
.define .lar4

.lar4:
				! ebx, descriptor address
				! eax, index
	pop	ecx
	pop	edx		! base address
	push	ecx
	sub     eax,(ebx)
	mov     ecx,8(ebx)
	imul    ecx
	add     edx,eax
	sar     ecx,1
	jnb     1f
	xor     eax,eax
	movb	al,(edx)
	pop	ebx
	push    eax
	jmp     ebx
1:
	sar     ecx,1
	jnb     1f
	xor     eax,eax
	o16 mov	ax,(edx)
	pop	ebx
	push    eax
	jmp     ebx
1:
	mov	edx,esi		! saved esi
	mov	eax,8(ebx)
	pop	ebx		! return address
	sub     esp,eax
	mov	eax,edi		! save edi
	mov     edi,esp
	rep movs
	mov	edi,eax
	mov	esi,edx
	jmp     ebx
