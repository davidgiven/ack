.define .cii

.cii:
        ! $Header$
	pop     di              ! return address
				! pop     cx, dest. size
				! pop     bx, src. size
				! ax is first word of source
	cmp     bx,cx
	je      8f
	cmp     bx,2
	je      1f
	cmp     bx,4
	jne     7f
	cmp     cx,2
	jne     9f
	pop     dx
8:
	jmp     di
7:      cmp     bx,1
	jne     9f
	cmp     cx,2
	jne     9f
	cbw
	jmp     8b
1:
	cmp     cx,4
	jne     9f
	cwd
	push    dx
	jmp     di
9:
	push    ax              ! push low source
	mov     ax,EILLINS
	push    ax
	jmp     .fat
	jmp     bx
