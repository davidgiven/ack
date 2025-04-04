#
    mes 2, EM_WSIZE, EM_PSIZE

/*
 * Tests _dup_ and _dus_ by loading 20 bytes from _src_, then making
 * and checking some duplicates.  The compilers might never _dup_ or
 * _dus_ with large sizes, so the compilers might work even if this
 * test fails.  You can cheat this test if _cms_ always pushes zero.
 */

    exa src
    exa size
src
    con 3593880729I4, 782166578I4, 4150666996I4, 2453272937I4, 3470523049I4
size
    con 20I2

    exp $check
    exp $_m_a_i_n
    pro $_m_a_i_n, 0

    /* Push 3 copies of src on stack. */
    lae src
    loi 20        /* 1st copy */
    dup 20        /* 2nd copy */
    lae size
    loi 2
    dus EM_WSIZE  /* 3rd copy */

    cal $check
    cal $finished
    end /* $_m_a_i_n */

    pro $check, 4 * EM_PSIZE + EM_WSIZE
#define p1    (-1 * EM_PSIZE)
#define p2    (-2 * EM_PSIZE)
#define p3    (-3 * EM_PSIZE)
#define p4    (-4 * EM_PSIZE)
#define i     (p4 - EM_WSIZE)

    /* Set pointers to all 4 copies. */
    lae src
    lal p4
    sti EM_PSIZE  /* p4 = src */
    lal 0
    lal p3
    sti EM_PSIZE  /* p3 = 3rd copy */
    lal 20
    lal p2
    sti EM_PSIZE  /* p2 = 2nd copy */
    lal 40
    lal p1
    sti EM_PSIZE  /* p1 = 1st copy */

    /* Loop 20 times to verify each byte. */
    loc 0
    stl i
4
    lal p4
    loi EM_PSIZE
    loi 1         /* byte from src */
    lal p3
    loi EM_PSIZE
    loi 1         /* byte from 3rd copy */
    cms EM_WSIZE
    zeq *3
    loc (3 * 256)
    lol i
    adi EM_WSIZE  /* 0x300 + i */
    loc EM_WSIZE
    loc 4
    cuu
    cal $fail
    asp 4
3
    lal p4
    loi EM_PSIZE
    loi 1         /* byte from src */
    lal p2
    loi EM_PSIZE
    loi 1         /* byte from 2nd copy */
    cms EM_WSIZE
    zeq *2
    loc (2 * 256)
    lol i
    adi EM_WSIZE  /* 0x200 + i */
    loc EM_WSIZE
    loc 4
    cuu
    cal $fail
    asp 4
2
    lal p4
    loi EM_PSIZE
    loi 1         /* byte from src */
    lal p1
    loi EM_PSIZE
    loi 1         /* byte from 1st copy */
    cms EM_WSIZE
    zeq *1
    loc (1 * 256)
    lol i
    adi EM_WSIZE  /* 0x100 + i */
    loc EM_WSIZE
    loc 4
    cuu
    cal $fail
    asp 4
1
    lal p4
    loi EM_PSIZE
    adp 1
    lal p4
    sti EM_PSIZE  /* increment p4 */
    lal p3
    loi EM_PSIZE
    adp 1
    lal p3
    sti EM_PSIZE  /* increment p3 */
    lal p2
    loi EM_PSIZE
    adp 1
    lal p2
    sti EM_PSIZE  /* increment p2 */
    lal p1
    loi EM_PSIZE
    adp 1
    lal p1
    sti EM_PSIZE  /* increment p1 */
    inl i
    lol i
    loc 20
    blt *4        /* loop 20 times */

    ret 0
    end /* $check */
