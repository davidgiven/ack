.define	endtext,enddata,endbss
.define _end,_etext,_edata

        ! $Header$
	.text
endtext:
_etext:
	.align 2
	.data
enddata:
_edata:
	.align 2
	.bss
_end:
endbss:
	.align 2
