#
    mes 2, _EM_WSIZE, _EM_PSIZE

/*
 * Tests _rol_ (rotate left) and _ror_ (rotate right).  Several back
 * ends provide _rol_ and _ror_, but as of year 2017, the compilers
 * and optimizers had never emit _rol_ nor _ror_.
 *
 * By tradition, _rol_ and _ror_ can't rotate values shorter than the
 * word size, or longer than 4 bytes.
 *  - If word size is 2, then try rotating 2-byte and 4-byte values.
 *  - If word size is 4, then try rotating 4-byte values.
 *
 * You can cheat this test if _cmu_ always pushes zero.
 */

#if _EM_WSIZE == 2
#define LEN2  4
    exa table2
    exa left2
    exa right2
table2         /* left, right */
    con 12715U2  /*  0,  0 */
    con 25430U2  /*  1, 15 */
    con 43825U2  /*  8,  8 */
    con 39125U2  /* 15,  1 */
left2
    con 0I2, 1I2, 8I2, 15I2
right2
    con 0I2, 15I2, 8I2, 1I2
#endif

#define LEN4  4
    exa table4
    exa left4
    exa right4
table4              /* left, right */
    con  437223536U4  /*  0,  0 */
    con  874447072U4  /*  1, 31 */
    con 2154830351U4  /* 16, 16 */
    con  218611768U4  /* 31,  1 */
left4
    con 0I2, 1I2, 16I2, 31I2
right4
    con 0I2, 31I2, 16I2, 1I2

    exa val4
    exa val4left7
    exa val4right11
val4
    con 4283808839U4
val4left7
    con 2866684927U4
val4right11
    con 2298473143U4

    exp $_m_a_i_n
    pro $_m_a_i_n, _EM_WSIZE
#define i -_EM_WSIZE

#if _EM_WSIZE == 2
    /*
     * Loop for LEN2 items in table2.
     */
    loc 0
    stl i
1
    lae table2
    loi 2         /* value to rotate */
    lae left2
    lol i
    loc 1
    sli _EM_WSIZE
    ads _EM_WSIZE
    loi 2         /* left distance */
    rol 2         /* rotate left */
    lae table2
    lol i
    loc 1
    sli _EM_WSIZE
    ads _EM_WSIZE
    loi 2         /* expected result */
    cmu 2
    zeq *2
    loc __LINE__
    loc _EM_WSIZE
    loc 4
    cuu
    cal $fail
    asp 4
2
    lae table2
    loi 2         /* value to rotate */
    lae right2
    lol i
    loc 1
    sli _EM_WSIZE
    ads _EM_WSIZE
    loi 2         /* right distance */
    ror 2         /* rotate right */
    lae table2
    lol i
    loc 1
    sli _EM_WSIZE
    ads _EM_WSIZE
    loi 2         /* expected result */
    cmu 2
    zeq *3
    loc __LINE__
    loc _EM_WSIZE
    loc 4
    cuu
    cal $fail
    asp 4
3
    inl i         /* loop LEN2 times */
    lol i
    loc LEN2
    blt *1
#endif /* _EM_WSIZE == 2 */

    /*
     * Loop for LEN4 items in table4.
     */
    loc 0
    stl i
4
    lae table4
    loi 4         /* value to rotate */
    lae left4
    lol i
    loc 1
    sli _EM_WSIZE
    ads _EM_WSIZE
    loi 2         /* left distance */
    loc 2
    loc _EM_WSIZE
    cii
    rol 4         /* rotate left */
    lae table4
    lol i
    loc 2
    sli _EM_WSIZE
    ads _EM_WSIZE
    loi 4         /* expected result */
    cmu 4
    zeq *5
    loc __LINE__
    loc _EM_WSIZE
    loc 4
    cuu
    cal $fail
    asp 4
5
    lae table4
    loi 4         /* value to rotate */
    lae right4
    lol i
    loc 1
    sli _EM_WSIZE
    ads _EM_WSIZE
    loi 2         /* right distance */
    loc 2
    loc _EM_WSIZE
    cii
    ror 4         /* rotate right */
    lae table4
    lol i
    loc 2
    sli _EM_WSIZE
    ads _EM_WSIZE
    loi 4         /* expected result */
    cmu 4
    zeq *6
    loc __LINE__
    loc _EM_WSIZE
    loc 4
    cuu
    cal $fail
    asp 4
6
    inl i         /* loop LEN4 times */
    lol i
    loc LEN4
    blt *4

    /*
     * Rotate 4-byte values by a constant distance, because this uses
     * different rules in PowerPC ncg.
     */
    lae val4
    loi 4
    loc 7
    rol 4         /* rotate left by 7 bits */
    lae val4left7
    loi 4
    cmu 4
    zeq *7
    loc __LINE__
    loc _EM_WSIZE
    loc 4
    cuu
    cal $fail
    asp 4
7
    lae val4
    loi 4
    loc 11
    ror 4         /* rotate right by 11 bits */
    lae val4right11
    loi 4
    cmu 4
    zeq *8
    loc __LINE__
    loc _EM_WSIZE
    loc 4
    cuu
    cal $fail
    asp 4
8

    cal $finished
    end
