.sect .text; .sect .rom; .sect .data; .sect .bss
.define .error
.define .Xtrp

	! eax is trap number
	! all registers must be saved
	! because return is possible
	! May only be called with error no's <16
.sect .text
.error:
	push ebp
	push esi
	push edi
	push edx
	push ecx
	push ebx
	push eax
	mov  ecx,eax
	mov  ebx,1
	sal  ebx,cl
.extern .ignmask
.extern .trp
	test ebx,(.ignmask)
	jne  2f
	call    .trp
2:
	pop  eax
	pop  ebx
	pop  ecx
	pop  edx
	pop  edi
	pop  esi
	pop  ebp
	ret

.Xtrp:
	cmp	eax,16
	jge	1f
	call	.error
	ret
1:
	jmp	.trp
