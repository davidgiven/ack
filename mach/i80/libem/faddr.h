.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text

! Fetches the word at positive stack offset OFFSET into de.

#define PASTE(a, b) a ## b
#define LABEL(prefix, offset) PASTE(prefix, offset)

.define LABEL(.faddr, OFFSET)
LABEL(.faddr, OFFSET):
    lhld .fp
    #if OFFSET == 0
    #elif OFFSET == 1
        inx h
    #elif OFFSET == 2
        inx h
        inx h
    #else
        lxi b, OFFSET
        dad b
    #endif
    ret
