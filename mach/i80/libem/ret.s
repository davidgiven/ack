.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text

.define .ret
.ret:
	mov h, b
	mov l, c
	sphl
	pop b
	ret

