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
    call LABEL(.faddr, OFFSET)
    mov e, m
    inx h
    mov d, m
    ret
