.global _sbrk, __sbrk

_sbrk:	
	set	__sbrk,%o7
	jmp	%o7
	nop
