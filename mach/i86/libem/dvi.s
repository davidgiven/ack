.define .dvi

        ! $Header$
        ! #bytes in ax
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
        call   .dvi4
        pop     bx
        push    cx
        push    ax
        jmp     bx
9:
        mov     ax,EODDZ
        push    ax
        jmp     .trp
