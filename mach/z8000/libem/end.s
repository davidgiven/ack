.define	endtext, enddata, endbss, _etext, _edata, _end

	.text
endtext:
_etext:
	.align 2
	.data
	.align 2
enddata:
_edata:
	.bss
	.align 2
endbss:
_end:
