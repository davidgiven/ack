.define	__brksize
.extern	endbss, __brksize
.sect	.text
.sect	.rom
.sect	.data
.sect	.bss

.sect	.data
__brksize:
.data4	endbss
