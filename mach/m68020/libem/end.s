.define	endtext,enddata,endbss,_etext,_edata,_end
.sect .text
	.align 4
.sect .rom
	.align 4
.sect .data
	.align 4
.sect .bss
	.align 4
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
