.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text

.define .ret
.ret:
	lhld .fp
	sphl
	pop h
	shld .fp
	ret

