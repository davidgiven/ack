#
.sect .text
.sect .rom
.sect .data
.sect .bss

.sect .text
.define .trap_ecase
.trap_ecase:
	li	r4, 20			! ECASE = 20 in h/em_abs.h
	! FALLTHROUGH to .trp

.define .trp
.trp:
	syscall 0

