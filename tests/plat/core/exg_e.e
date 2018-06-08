#
    mes 2, EM_WSIZE, EM_PSIZE

/*
 * Tests _exg_ by loading 40 bytes from _src_, then exchanging 20 and
 * 20 bytes, and checking the result.  The compilers might never _exg_
 * large sizes, so the compilers might work even if this test fails.
 * You can cheat this test if _cms_ always pushes zero.
 */

    exa src
src
    con 1539465570I4, 1344465418I4, 1317578918I4, 1163467696I4, 2645261331I4
    con 3981585269I4, 1433968975I4, 4256886989I4, 4114909542I4, 1817334375I4

    exp $check
    exp $_m_a_i_n
    pro $_m_a_i_n, 0

    lae src
    loi 40
    exg 20
    cal $check
    cal $finished
    end /* $_m_a_i_n */

    pro $check, 2 * EM_PSIZE + EM_WSIZE
#define p1    (-1 * EM_PSIZE)
#define p2    (-2 * EM_PSIZE)
#define i     (p2 - EM_WSIZE)

    lae src
    lal p2
    sti EM_PSIZE  /* p2 = src */
    lal 0
    adp 20
    lal p1
    sti EM_PSIZE  /* p1 = exchanged copy + 20 */

    /* Loop 40 times to verify each byte. */
    loc 0
    stl i
1
    lal p2
    loi EM_PSIZE
    loi 1         /* byte from src */
    lal p1
    loi EM_PSIZE
    loi 1         /* byte from exchanged copy */
    cms EM_WSIZE
    zeq *2
    lol i
    loc EM_WSIZE
    loc 4
    cuu
    cal $fail
    asp 4
2
    lal p2
    loi EM_PSIZE
    adp 1
    lal p2
    sti EM_PSIZE  /* increment p2 */
    lal p1
    loi EM_PSIZE  /* p1 */
    inl i
    /* When i reaches 20, p1 would reach end of exchanged copy. */
    lol i
    loc 20
    beq *3
    adp 1         /* p1 + 1 */
    bra *4
3
    adp -39       /* p1 - 39, beginning of exchanged copy */
4
    lal p1
    sti EM_PSIZE  /* move p1 */
    lol i
    loc 40
    blt *1

    ret 0
    end /* $check */