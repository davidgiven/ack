.define EARRAY,ERANGE,ESET,EIOVFL
.define ECONV,ESTACK
.define EHEAP,EODDZ,ECASE
.define EBADMON,EBADLIN,EBADGTO
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text

! This file contains the global data used by the trap routine.


! DATA
.sect .data
EARRAY:
.asciz	"Array bound error\n\r"
ERANGE:
.asciz	"Range bound error\n\r"
ESET:
.asciz	"Set bound error\n\r"
EIOVFL:
.asciz	"Integer overflow\n\r"
ECONV:
.asciz	"Conversion error\n\r"
ESTACK:
.asciz	"Stack overflow\n\r"
EHEAP:
.asciz	"Heap overflow\n\r"
EODDZ:
.asciz	"Illegal size argument\n\r"
ECASE:
.asciz	"Case error\n\r"
EBADMON:
.asciz	"Bad monitor call\n\r"
EBADLIN:
.asciz	"Argument of LIN to high\n\r"
EBADGTO:
.asciz	"GTO descriptor error\n\r"


