.sect .text; .sect .rom; .sect .data; .sect .bss
.sect .text
.define .mli

	! #bytes in ax
.mli:
	pop     bx              ! return address
	cmp     ax,2
	jne     1f
	pop     ax
	pop     cx
	mul     cx
	push    ax
	jmp     bx
1:
	cmp     ax,4
	jne     9f
	push	bx
	mov	cx,bx
	mov	bx,sp
	mov	ax,2(bx)
	mov	(bx),ax
	mov	ax,4(bx)
	mov	2(bx),ax
	mov	ax,6(bx)
	mov	4(bx),ax
	mov	ax,8(bx)
	mov	6(bx),ax
	mov	8(bx),cx
	pop	ax
	pop	dx
	call   .mli4
	pop	bx
	push	dx
	push	ax
	jmp	bx
9:
.extern EODDZ
.extern .trp
	mov     ax,EODDZ
	push    bx
	jmp     .trp
