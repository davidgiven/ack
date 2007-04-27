! $Source$
! $State$
! $Revision$

.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .end ! only for declaration of _end, __end and endbss.
.define	endtext, endrom, enddata, endbss, __end

	.sect .text
endtext:
	.sect .rom
endrom:
	.sect .data
enddata:
	.sect .end
__end:
endbss:
