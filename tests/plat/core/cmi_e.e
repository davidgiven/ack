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
big
    #if EM_WSIZE == 2
        rom 32767
    #else
        rom 2147483647
    #endif

	/* 0 < 1 */

    loe zero
    loe one
    blt *1
    cmi EM_WSIZE
    zlt *1

    loc __LINE__
    cal $fail
    asp EM_WSIZE
1

    /* 1 > 0 */

    loe one
    loe zero
    cmi EM_WSIZE
    zgt *2

    loc __LINE__
    cal $fail
    asp EM_WSIZE
2

    /* 0 <= 0 */

    loe zero
    loe zero
    cmi EM_WSIZE
    zle *3

    loc __LINE__
    cal $fail
    asp EM_WSIZE
3

    /* 0 >= 0 */

    loe zero
    loe zero
    cmi EM_WSIZE
    zge *4

    loc __LINE__
    cal $fail
    asp EM_WSIZE
4

	/* INT_MIN < 1 */

    loe mbig
    loe one
    cmi EM_WSIZE
    zlt *5

    loc __LINE__
    cal $fail
    asp EM_WSIZE
5

    /* 1 > INT_MIN */

    loe one
    loe mbig
    cmi EM_WSIZE
    zgt *6

    loc __LINE__
    cal $fail
    asp EM_WSIZE
6

    /* INT_MIN < INT_MAX */

    loe mbig
    loe big
    cmi EM_WSIZE
    zlt *7
    
    loc __LINE__
    cal $fail
    asp EM_WSIZE
7

    cal $finished
    end


