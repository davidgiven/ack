.define	endtext,enddata,endbss,_etext,_edata,_end
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .end ! only for declaration of _end and endbss.

	.sect .text
endtext:
_etext:
	.sect .data
enddata:
_edata:
	.sect .end
_end:
endbss:
