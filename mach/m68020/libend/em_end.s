! $Source: /cvsroot/tack/Ack/mach/i386/libend/em_end.s,v $
! $State: Exp $
! $Revision: 1.2 $

.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .end ! only for declaration of _end, __end and endbss.
.define	endtext, endrom, enddata, endbss, __end

	.sect .text
	.align 4
endtext:
	.sect .rom
	.align 4
endrom:
	.sect .data
	.align 4
enddata:
	.sect .end
	.align 4
__end:
endbss:
