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
		#error bad word size
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
    #if W == 2
        rom 32767
    #elif W == 3
		rom 8388607
	#elif W == 4
        rom 2147483647
	#else
		#error bad word size
    #endif

	/* 0 < 1 */

    loe zero
    loe one
    cmu W
    zlt *1

    loc __LINE__
    cal $fail
    asp W
1

    /* 1 > 0 */

    loe one
    loe zero
    cmu W
    zgt *2

    loc __LINE__
    cal $fail
    asp W
2

    /* 0 <= 0 */

    loe zero
    loe zero
    cmu W
    zle *3

    loc __LINE__
    cal $fail
    asp W
3

    /* 0 >= 0 */

    loe zero
    loe zero
    cmu W
    zge *4

    loc __LINE__
    cal $fail
    asp W
4

	/* INT_MIN > 1 */

    loe mbig
    loe one
    cmu W
    zgt *5

    loc __LINE__
    cal $fail
    asp W
5

    /* 1 < INT_MIN */

    loe one
    loe mbig
    cmu W
    zlt *6

    loc __LINE__
    cal $fail
    asp W
6

    /* INT_MIN > INT_MAX */

    loe mbig
    loe big
    cmu W
    zgt *7
    
    loc __LINE__
    cal $fail
    asp W
7

    /* Test case: twofivesix < 256 */

    loe twofivesix
    loc 256
    cmu W
    zlt *8
    bra *9
8
    loc __LINE__
    cal $fail
    asp W
9

    cal $finished
    end
