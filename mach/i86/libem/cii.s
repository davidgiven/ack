.sect .text; .sect .rom; .sect .data; .sect .bss
.define .cii

.sect .text
.cii:
	pop     di              ! return address
				! pop     cx, dest. size
				! pop     bx, src. size
				! ax is first word of source
	cmp	bx,1
	jne	2f
	cbw
	mov	bx,2
2:
	cmp     bx,cx
	je      8f
	cmp     bx,2
	je      1f
	cmp     bx,4
	jne     9f
	cmp     cx,2
	jne     9f
	pop     dx
8:
	jmp     di
1:
	cmp     cx,4
	jne     9f
	cwd
	push    dx
	jmp     di
9:
	push    ax              ! push low source
.extern EILLINS
.extern .fat
	mov     ax,EILLINS
	push    ax
	jmp     .fat
	jmp     bx
