.sect .text; .sect .rom; .sect .data; .sect .bss
.define .dvi

        ! #bytes in ax
	.sect .text
.dvi:
        pop     bx              ! return address
        cmp     ax,2
        jne     1f
        pop     ax
        cwd
        pop     cx
        idiv    cx
        push    ax
        jmp     bx
1:
        cmp     ax,4
        jne     9f
        pop     ax
        pop     dx
        pop     si
        pop     di
        push    bx
        push    di
        push    si
        push    dx
        push    ax
.extern .dvi4
        call   .dvi4
        pop     bx
        push    dx
        push    ax
        jmp     bx
9:
.extern EODDZ
.extern .trp
        mov     ax,EODDZ
        push    bx
        jmp     .trp
