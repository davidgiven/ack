.define	endtext,enddata,endbss
.define _end,_etext,_edata

        ! $Header$
	.text
	.align 2
endtext:
_etext:
	.data
	.align 2
enddata:
_edata:
	.bss
	.align 2
_end:
endbss:
