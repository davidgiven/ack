.global _brk, __brk

_brk:	
	set	__brk,%o0
	jmp	%o0
	nop
