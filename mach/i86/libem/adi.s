.sect .text; .sect .rom; .sect .data; .sect .bss
.define .adi

	! #bytes in cx , top of stack in ax
	.sect .text
.adi:
	pop     bx              ! return address
	cmp     cx,2
	jne     1f
	pop     cx
	add     ax,cx
	jmp     bx
1:
	cmp     cx,4
	jne     9f
	pop     dx
	pop     cx
	add     ax,cx
	pop     cx
	adc     dx,cx
	push    dx
	jmp     bx
9:
.extern	EODDZ
.extern .trp
	mov     ax,EODDZ
	push    ax
	jmp     .trp
