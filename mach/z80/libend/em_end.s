.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .end ! only for declaration of _end, __end and endbss.
.define	endtext,enddata,endbss,__end

	.sect .text
endtext:
	.sect .data
enddata:
	.sect .end
__end:
endbss:
