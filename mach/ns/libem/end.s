.define	endtext,enddata,endbss
.define _end, _etext, _edata

	.text
endtext:
_etext:
	.align	4
	.data
enddata:
_edata:
	.align	4
	.bss
	.align	4
endbss:
_end:
