.sect .text; .sect .rom; .sect .data; .sect .bss
.sect .text
.define .sar4

.sar4:
				! ebx, descriptor address
				! eax, index
	pop	ecx
	pop	edx		! base address
	push	ecx
	sub     eax,(ebx)
	mov     ecx,8(ebx)
	push	edx
	imul    ecx
	pop	edx
	add     edx,eax
	sar     ecx,1
	jnb     1f
	pop	ebx
	pop     eax
	movb	(edx),al
	jmp     ebx
1:
	sar     ecx,1
	jnb     1f
	pop	ebx
	pop     eax
	o16 mov (edx),ax
	jmp     ebx
1:
	pop	ebx
	xchg	edi,edx		! edi = base address, edx is saved edi
	mov	eax,esi
	mov     esi,esp
	rep movs
	mov     esp,esi
	mov	esi,eax
	mov	edi,edx
	jmp     ebx
