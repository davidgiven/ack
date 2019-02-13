.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text

! Fetches the word at positive stack offset OFFSET into de.

#define PASTE(a, b) a ## b
#define LABEL(prefix, offset) PASTE(prefix, offset)

.define LABEL(.fload, OFFSET)
LABEL(.fload, OFFSET):
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
    mov e, m
    inx h
    mov d, m
    ret
