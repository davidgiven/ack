.define unknown
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text

unknown:
	push	*RR14, $EODDZ
	jr	fatal
