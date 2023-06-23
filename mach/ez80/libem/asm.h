.sect .text
.sect .rom
.sect .data
.sect .bss

#define LOGICOP(opcode, size) \
        push ix;            \
        push bc;            \
        push hl;            \
                            \
        ld ix, 0;           \
        add ix, sp;         \
                            \
        ld c, size;         \
    loop:;                  \
        ld a, (ix+0);       \
        opcode (ix+size);   \
        ld (ix+0), a;       \
        inc ix;             \
        djnz loop;          \
                            \
        pop hl;             \
        pop bc;             \
        pop ix;             \
        ret

// vim: ts=4 sw=4 et

