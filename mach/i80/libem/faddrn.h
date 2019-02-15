.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text

! Fetches the word at negative stack offset OFFSET into de.

#define PASTE(a, b) a ## b
#define LABEL(prefix, offset) PASTE(prefix, offset)

.define LABEL(.faddrn, OFFSET)
LABEL(.faddrn, OFFSET):
    lhld .fp
    #if OFFSET == 0
    #elif OFFSET == 1
        dcx h
    #elif OFFSET == 2
        dcx h
        dcx h
    #else
        lxi b, -OFFSET
        dad b
    #endif
    ret
