#
    mes 2, EM_WSIZE, EM_PSIZE

/*
 * Tests _dup_ and _dus_ by loading SIZE bytes from _src_, then making
 * and checking some duplicates.  The compilers might never _dup_ or
 * _dus_ with large sizes, so the compilers might work even if this
 * test fails.  You can cheat this test if _cms_ always pushes zero.
 */

    exa src
    exa size
src
    con 3593880729I4, 782166578I4, 4150666996I4, 2453272937I4, 3470523049I4
size
#if EM_WSIZE == 2
	con 20I2
	#define SIZE 20
#elif EM_WSIZE == 3
	con 21I3
	#define SIZE 21
#elif EM_WSIZE == 4
	com 20I2
	#define SIZE 20
#else
	#error unsupported word size
#endif

    exp $check
    exp $_m_a_i_n
    pro $_m_a_i_n, 0

    /* Push 3 copies of src on stack. */
    lae src
    loi SIZE      /* 1st copy */
    dup SIZE      /* 2nd copy */
    lae size
    loi EM_WSIZE
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
    lal SIZE * 1
    lal p2
    sti EM_PSIZE  /* p2 = 2nd copy */
    lal SIZE * 2
    lal p1
    sti EM_PSIZE  /* p1 = 1st copy */

    /* Loop SIZE times to verify each byte. */
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
    cal $fail
    asp EM_WSIZE
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
    cal $fail
    asp EM_WSIZE
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
    cal $fail
    asp EM_WSIZE
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
    loc SIZE
    blt *4        /* loop SIZE times */

    ret 0
    end /* $check */

// vim: sw=4 ts=4 et ft=asm

