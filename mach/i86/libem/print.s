.sect .text; .sect .rom; .sect .data; .sect .bss
.sect .text
.define	printc,printd,prints

	! argument in ax
	! uses bx
prints:
	xchg	ax,bx
1:
	movb	al,(bx)
	inc	bx
	testb	al,al
	jz	2f
	call	printc
	jmp	1b
2:
	ret

	! argument in ax
	! uses cx and dx
printd:
	xor	dx,dx
	mov	cx,10
	div	cx
	test	ax,ax
	jz	1f
	push	dx
	call	printd
	pop	dx
1:
	xchg	ax,dx
	addb	al,'0'

	! argument in ax
printc:
	push	ax
	mov	bx,sp
	mov	ax,1
	push	ax
	push	bx
	push	ax
	call	__write
	pop	bx
	pop	bx
	pop	bx
	pop	bx
	ret
