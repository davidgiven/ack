.sect .text; .sect .rom; .sect .data; .sect .bss
.sect .text
.define .loi
.define .los

	! #bytes in ecx
	! address in ebx
	! save esi/edi. they might be register variables
.los:
	pop	edx
	mov     eax,ecx
	sar     ecx,1
	jnb     1f
	xor	eax,eax
	movb	al,(edx)
	push    eax
	jmp     edx
1:
	sar     ecx,1
	jnb     1f
	xor	eax,eax
	o16 mov	ax,(edx)
	push    eax
	jmp     edx
1:
	mov	ebx,edx
	mov	edx,esi
	sub     esp,eax
	jmp	1f

.loi:
	! only called with size >= 4
	mov	edx,esi
	mov	esi,ebx
	pop	ebx
	sub	esp,ecx
	sar	ecx,2
1:
	mov	eax,edi
	mov     edi,esp
	rep movs
	mov	esi,edx
	mov	edi,eax
	jmp     ebx
