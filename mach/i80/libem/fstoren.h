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
    call LABEL(.faddr, OFFSET)
    mov m, e
    inx h
    mov m, d
    ret

