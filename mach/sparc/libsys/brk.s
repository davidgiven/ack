.global _brk, __brk

_brk:	
	set	__brk,%o7
	jmp	%o7
	nop
