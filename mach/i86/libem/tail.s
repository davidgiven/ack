.define	endtext,enddata,endbss
.define _end,_etext,_edata

        ! $Header$
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
_end:
endbss:
