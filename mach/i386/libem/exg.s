.sect .text; .sect .rom; .sect .data; .sect .bss
.define .exg

	! #bytes in ecx
.sect .text
.exg:
	push	edi
	mov	esp,edi
	add	edi,8
	mov	ebx,edi
	add	ebx,ecx
	sar     ecx,2
1:
	mov	eax,(ebx)
	xchg	eax,(edi)
	mov	(ebx),eax
	loop	1b
2:
	pop	edi
	ret
