#
    mes 2, EM_WSIZE, EM_PSIZE

    exp $_m_a_i_n
    pro $_m_a_i_n, 0

    /* Test non-existent bit */

.1
    rom 0I4
    loe .1
    loc 1       /* bit number */
    inn 4
    zeq *1

    loc __LINE__
    loc EM_WSIZE
    loc 4
    cuu
    cal $fail
    ass EM_WSIZE
1

    /* Test existent bit */

.2
    rom 16384
.21
    rom 14      /* to defeat constant folding */

    loe .2
    loe .21      /* bit number */
    inn EM_WSIZE
    zne *2

    loc __LINE__
    loc EM_WSIZE
    loc 4
    cuu
    cal $fail
    ass EM_WSIZE
2

    /* Test non-existent high bit */

.3
    rom 0, 0
.31
    rom 8       /* to defeat constant folding */

    lae .3
    loi EM_WSIZE*2
    loe .31     /* bit number */
    inn EM_WSIZE*2
    zeq *3

    loc __LINE__
    loc EM_WSIZE
    loc 4
    cuu
    cal $fail
    ass EM_WSIZE
3

    /* Test existent high bit */

.4
    rom 0, 2
.41
    rom (EM_WSIZE*8)+1      /* to defeat constant folding */

    lae .4
    loi EM_WSIZE*2
    loe .41     /* bit number */
    inn EM_WSIZE*2
    zne *4

    loc __LINE__
    loc EM_WSIZE
    loc 4
    cuu
    cal $fail
    ass EM_WSIZE
4

    cal $finished
    end
