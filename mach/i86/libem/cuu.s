.sect .text; .sect .rom; .sect .data; .sect .bss
.define .ciu
.define .cui
.define .cuu

.sect .text
.ciu:
.cui:
.cuu:
	pop     di              ! return address
				! pop     cx, dest. size
				! pop     bx, source size
				! ax is low word of source
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
	xor     dx,dx
	push    dx
	jmp     di
9:
	push    ax              ! to help debugging ?
.extern EILLINS
.extern .fat
	mov     ax,EILLINS
	push    ax
	jmp     .fat
	jmp     bx
