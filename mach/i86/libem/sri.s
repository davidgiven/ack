.sect .text; .sect .rom; .sect .data; .sect .bss
.sect .text
.define .sri

	! #bytes in ax
.sri:
	pop     dx              ! return address
	cmp     ax,2
	jne     1f
	pop     ax
	pop     cx
	sar     ax,cl
	push    ax
	jmp     dx
1:
	cmp     ax,4
	jne     9f
	pop     cx
	jcxz    2f
	pop     ax
	pop     bx
3:
	sar     bx,1
	rcr     ax,1
	loop    3b
	push    bx
	push    ax
2:
	jmp     dx
9:
.extern EODDZ
.extern .trp
	mov     ax,EODDZ
	push    ax
	jmp     .trp
