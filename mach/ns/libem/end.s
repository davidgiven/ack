.define	endtext,enddata,endbss
.define _end, _etext, _edata
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text

	.sect .text
endtext:
_etext:
	.align	4
	.sect .data
enddata:
_edata:
	.align	4
	.sect .bss
	.align	4
endbss:
_end:
