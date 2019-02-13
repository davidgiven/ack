.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text

! Stores the word in de to positive stack offset OFFSET.

#define PASTE(a, b) a ## b
#define LABEL(prefix, offset) PASTE(prefix, offset)

.define LABEL(.fstore, OFFSET)
LABEL(.fstore, OFFSET):
    #if OFFSET == 0
        #error "0 shouldn't happen"
    #elif OFFSET == 1
        mov l, c
        mov h, b
        inx h
    #elif OFFSET == 2
        mov l, c
        mov h, b
        inx h
        inx h
    #else
        lxi h, OFFSET
        dad b
    #endif
    mov m, e
    inx h
    mov m, d
    ret
