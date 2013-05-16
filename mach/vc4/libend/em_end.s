#
/*
 * VideoCore IV support library for the ACK
 * © 2013 David Given
 * This file is redistributable under the terms of the 3-clause BSD license.
 * See the file 'Copying' in the root of the distribution for the full text.
 */

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
