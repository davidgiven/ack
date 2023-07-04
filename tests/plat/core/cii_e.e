#
#define W EM_WSIZE
#define P EM_PSIZE
    mes 2, W, P

    exp $_m_a_i_n
    pro $_m_a_i_n, 0

onebyte
    rom 1I1
oneshort
	rom 1
moneshort
    rom -1
monebyte
    rom 255I1

    /* Positive byte->word */

    lae onebyte
	loi 1
    loc 1
    loc W
    cii
    loc 1
	cmu W
	zeq *1

    loc __LINE__
    cal $fail
1

    /* Negative byte->word */

    lae monebyte
	loi 1
    loc 1
    loc W
    cii
    loc -1
	cmu W
	zeq *2

    loc __LINE__
    cal $fail
2

#if (W == 2) || (W == 3)
    /* Positive byte->dword */

    lae onebyte
	loi 1
    loc 1
    loc W*2
    cii
    ldc 1
	cmu W*2
	zeq *3

    loc __LINE__
    cal $fail
3

    /* Negative byte->short */

    lae monebyte
	loi 1
    loc 1
    loc W*2
    cii
    ldc -1
	cmu W*2
	zeq *4

    loc __LINE__
    cal $fail
4
    /* Positive word->dword */

    loe oneshort
    loc W
    loc W*2
    cii
    ldc 1
	cmu W*2
	zeq *5

    loc __LINE__
    cal $fail
5

    /* Negative word->dword */

    loe moneshort
    loc W
    loc W*2
    cii
    ldc -1
	cmu W*2
	zeq *6

    loc __LINE__
    cal $fail
6
#endif

    cal $finished
    end

