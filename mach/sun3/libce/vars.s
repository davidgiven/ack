.define .lino,.filn
.define	hol0,.reghp,.limhp,.trpim,.trppc
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .data
hol0:
.lino:
	.data4	0	! lino
.filn:
	.data4	0	! filn
.reghp:
	.data4	_end
.limhp:
	.data4	_end
.trppc:
	.data4	0
.trpim:
	.data4	0	! USED TO BE 2 BYTES; IS THIS RIGHT?
