.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text

! Fetches the word at negative stack offset OFFSET into de.

#define PASTE(a, b) a ## b
#define LABEL(prefix, offset) PASTE(prefix, offset)

.define LABEL(.floadn, OFFSET)
LABEL(.floadn, OFFSET):
    #if OFFSET == 0
        #error "0 shouldn't happen"
    #elif OFFSET == 1
        mov l, c
        mov h, b
        mov d, m
        dcx h
        mov e, m
        ret
    #elif OFFSET == 2
        mov l, c
        mov h, b
        dcx h
        mov d, m
        dcx h
        mov e, m
        ret
    #else
        lxi h, -OFFSET
        dad b
        mov e, m
        inx h
        mov d, m
        ret
    #endif
