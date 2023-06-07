.sect .text
.sect .rom
.sect .data
.sect .bss

#define LOGICOP(opcode, size) \
        push af;            \
        push bc;            \
        push de;            \
        push hl;            \
                            \
        ld ix, 0;           \
        add ix, sp;         \
                            \
        ld b, size;         \
    loop:;                  \
        ld a, (ix+0);       \
        opcode (ix+size);   \
        ld (ix+0), a;       \
        inc ix;             \
        djnz loop;          \
                            \
        pop hl;             \
        pop de;             \
        pop bc;             \
        pop af;             \
        ret

// vim: ts=4 sw=4 et

