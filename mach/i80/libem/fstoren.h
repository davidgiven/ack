.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text

! Stores the word in de to negative stack offset OFFSET.

#define PASTE(a, b) a ## b
#define LABEL(prefix, offset) PASTE(prefix, offset)

.define LABEL(.fstoren, OFFSET)
LABEL(.fstoren, OFFSET):
    #if OFFSET == 0
        #error "0 shouldn't happen"
    #elif OFFSET == 1
        mov l, c
        mov h, b
        mov m, d
        dcx h
        mov m, e
        ret
    #elif OFFSET == 2
        mov l, c
        mov h, b
        dcx h
        mov m, d
        dcx h
        mov m, e
        ret
    #else
        lxi h, -OFFSET
        dad b
        mov m, e
        inx h
        mov m, d
        ret
    #endif

