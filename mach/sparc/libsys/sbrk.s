.global _sbrk, __sbrk

_sbrk:	
	set	__sbrk,%o0
	jmp	%o0
	nop
