.sect .text; .sect .rom; .sect .data; .sect .bss
.sect .text
.define .ror

	! #bytes in ax
.ror:
	pop     dx              ! return address
	cmp     ax,2
	jne     1f
	pop     ax
	pop     cx
	ror     ax,cl
	push    ax
	jmp     dx
1:
	cmp     ax,4
	jne     9f
	pop     cx
	jcxz    2f
	neg     cx
	add     cx,32
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
