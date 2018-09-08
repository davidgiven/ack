.sect .text
.sect .rom
.sect .data
.sect .bss

.define .trap_ecase
.trap_ecase:
	li	r3, 20			! ECASE = 20 in h/em_abs.h
	! FALLTHROUGH to .trp

.define .trp
.trp:
	syscall 0

