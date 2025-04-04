#
    mes 2, EM_WSIZE, EM_PSIZE

    exp $_m_a_i_n
    pro $_m_a_i_n, 0

oneshort
onebyte
    rom 1
moneshort
    rom 65535
monebyte
    rom 255

    /* Positive byte->word */

    loe onebyte
    loc 1
    loc EM_WSIZE
    cii
    loc 1
	cmu EM_WSIZE
	zeq *1

    loc __LINE__
    cal $fail
    asp 4
1

    /* Negative byte->word */

    loe monebyte
    loc 1
    loc EM_WSIZE
    cii
    loc -1
	cmu EM_WSIZE
	zeq *2

    loc __LINE__
    cal $fail
    asp 4
2

#if EM_WSIZE == 2
    /* Positive byte->dword */

    loe onebyte
    loc 1
    loc 4
    cii
    loc 0
    loc 1
	cmu 4
	zeq *3

    loc __LINE__
    cal $fail
    asp 4
3

    /* Negative byte->short */

    loe monebyte
    loc 1
    loc 4
    cii
    loc -1
    loc -1
	cmu 4
	zeq *4

    loc __LINE__
    cal $fail
    asp 4
4
    /* Positive word->dword */

    loe oneshort
    loc 2
    loc 4
    cii
    loc 0
    loc 1
	cmu 4
	zeq *5

    loc __LINE__
    cal $fail
    asp 4
5

    /* Negative word->short */

    loe moneshort
    loc 2
    loc 4
    cii
    loc -1
    loc -1
	cmu 4
	zeq *6

    loc __LINE__
    cal $fail
    asp 4
6
#endif

    cal $finished
    end

