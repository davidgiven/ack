#
	.define	_brksize
	.extern	_end
#ifdef ACK
	.sect	.text
	.sect	.rom
	.sect	.data
	.sect	.bss
#endif ACK

	.sect	.data
_brksize:
	.data4	_end
