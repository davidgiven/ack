#
    mes 2, EM_WSIZE, EM_PSIZE

    exp $_m_a_i_n
    pro $_m_a_i_n, 0

mbig
    #if EM_WSIZE == 2
        rom 32768
    #else
        rom 2147483648
    #endif
mone
    rom -1
zero
    rom 0
one
    rom 1
twofivesix
    rom 256
big
    #if EM_WSIZE == 2
        rom 32767
    #else
        rom 2147483647
    #endif

	/* 0 < 1 */

    loe zero
    loe one
    cmu EM_WSIZE
    zlt *1

    loc __LINE__
    cal $fail
    asp EM_WSIZE
1

    /* 1 > 0 */

    loe one
    loe zero
    cmu EM_WSIZE
    zgt *2

    loc __LINE__
    cal $fail
    asp EM_WSIZE
2

    /* 0 <= 0 */

    loe zero
    loe zero
    cmu EM_WSIZE
    zle *3

    loc __LINE__
    cal $fail
    asp EM_WSIZE
3

    /* 0 >= 0 */

    loe zero
    loe zero
    cmu EM_WSIZE
    zge *4

    loc __LINE__
    cal $fail
    asp EM_WSIZE
4

	/* INT_MIN > 1 */

    loe mbig
    loe one
    cmu EM_WSIZE
    zgt *5

    loc __LINE__
    cal $fail
    asp EM_WSIZE
5

    /* 1 < INT_MIN */

    loe one
    loe mbig
    cmu EM_WSIZE
    zlt *6

    loc __LINE__
    cal $fail
    asp EM_WSIZE
6

    /* INT_MIN > INT_MAX */

    loe mbig
    loe big
    cmu EM_WSIZE
    zgt *7
    
    loc __LINE__
    cal $fail
    asp EM_WSIZE
7

    /* Test case: twofixsix < 256 */

    loe twofivesix
    loc 256
    cmu EM_WSIZE
    zlt *8
    bra *9
8
    loc __LINE__
    cal $fail
    asp EM_WSIZE
9

    cal $finished
    end
