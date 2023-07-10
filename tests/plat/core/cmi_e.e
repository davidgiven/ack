#
#define W EM_WSIZE
#define P EM_PSIZE
    mes 2, W, P

    exp $_m_a_i_n
    pro $_m_a_i_n, 0

mbig
    #if W == 2
        rom 32768
    #elif W == 3
		rom 8388608
	#elif W == 4
        rom 2147483648
	#else
		#error Bad word size
    #endif
mone
    rom -1
zero
    rom 0
one
    rom 1
big
    #if W == 2
        rom 32767
    #elif W == 3
		rom 8388607
    #elif W == 4
        rom 2147483647
	#else
		#error Bad word size
    #endif

	/* 0 < 1 */

    loe zero
    loe one
    blt *1
    cmi W
    zlt *1

    loc __LINE__
    cal $fail
    asp W
1

    /* 1 > 0 */

    loe one
    loe zero
    cmi W
    zgt *2

    loc __LINE__
    cal $fail
    asp W
2

    /* 0 <= 0 */

    loe zero
    loe zero
    cmi W
    zle *3

    loc __LINE__
    cal $fail
    asp W
3

    /* 0 >= 0 */

    loe zero
    loe zero
    cmi W
    zge *4

    loc __LINE__
    cal $fail
    asp W
4

	/* INT_MIN < 1 */

    loe mbig
    loe one
    cmi W
    zlt *5

    loc __LINE__
    cal $fail
    asp W
5

    /* 1 > INT_MIN */

    loe one
    loe mbig
    cmi W
    zgt *6

    loc __LINE__
    cal $fail
    asp W
6

    /* INT_MIN < INT_MAX */

    loe mbig
    loe big
    cmi W
    zlt *7
    
    loc __LINE__
    cal $fail
    asp W
7

    cal $finished
    end


