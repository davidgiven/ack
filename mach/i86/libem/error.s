.define .error

        ! $Header$
	! ax is trap number
	! all registers must be saved
	! because return is possible
	! May only be called with error no's <16
.error:
	push bp
	push si
	push di
	push dx
	push cx
	push bx
	push ax
	mov  cx,ax
	mov  bx,1
	sal  bx,cl
	test bx,(.ignmask)
	jne  2f
	call    .trp
2:
	pop  ax
	pop  bx
	pop  cx
	pop  dx
	pop  di
	pop  si
	pop  bp
	ret
