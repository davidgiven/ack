.define begtext,begdata,begbss
.define hol0,.reghp,.limhp,.trppc,.ignmask
.define ERANGE,ESET,EHEAP,ECASE,EILLINS

ERANGE          = 1
ESET            = 2
EIDIVZ          = 6
EHEAP           = 17
EILLINS         = 18
EODDZ           = 19
ECASE           = 20

base            = 0x01C0
topmem          = 0xFFF0

	.org    topmem-16
.extern __n_line
maxmem:
__n_line:
	.space  16
	.errnz  __n_line-0xFFE0

	.base   base

	.text
begtext:
	cld
	xor     ax,ax
	mov     ss,ax
	mov     ds,ax
	mov     es,ax
	mov     (2),cs
	mov     (0),.diverr
	mov     sp,maxmem
	mov     di,begbss
	mov     cx,[[endbss-begbss]/2]&0x7FFF
	! xor     ax,ax ! ax still is 0
	rep stos
	mov     ax,envp
	push    ax
	mov     ax,argv
	push    ax
	mov     ax,1
	push    ax
	call    _m_a_i_n
	call	.stop
.diverr:
	push    ax
	mov     ax,EIDIVZ
	call    .error
	pop     ax
	iret

	.data
begdata:
hol0:
	.word   0,0
	.word   0,0
argv:
	.word   3f
envp:
	.word   0
3:
	.asciz "PROGRAM"
.reghp:
	.word   endbss
.limhp:
	.word   endbss
.ignmask:
	.word   0
.trppc:
	.word   0

	.bss
begbss:
