! Declare segments (the order is important).

.sect .text
.sect .rom
.sect .data
.sect .bss

.sect .text

.extern realloc_ptr

.define _brk
_brk:
	enter 0, 0
newsize = 2*4

	mov eax, newsize(ebp)
	callf (realloc_ptr)
	xor eax, eax
	leave
	ret


