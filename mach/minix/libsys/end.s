.sect .text; .sect .rom; .sect .data; .sect .bss
.define	endtext,enddata,endbss
.define _end,_etext,_edata

        ! $Header$
	.sect .text
endtext:
_etext:
	.align 2
	.sect .data
enddata:
_edata:
	.align 2
.sect .endsect
_end:
endbss:
	.align 2
