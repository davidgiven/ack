.sect .text
.sect .rom
.sect .data
.sect .bss
.define	endtext,enddata,endbss,__end
.sect .text
	.align 4
.sect .rom
	.align 4
.sect .data
	.align 4
.sect .bss
	.align 4
.sect .end ! only for declaration of _end, __end and endbss.

	.sect .text
endtext:
	.sect .data
enddata:
	.sect .end
__end:
endbss:
