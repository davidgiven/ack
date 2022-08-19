! Declare segments (the order is important).

.sect .text
.sect .rom
.sect .data
.sect .bss

.sect .text

.define _brk
_brk:
	mov ebx, esp
	mov eax, 1*4(ebx)
	callf (realloc_ptr)
	xor eax, eax
	ret


