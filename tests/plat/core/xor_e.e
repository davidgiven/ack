#
    mes 2, EM_WSIZE, EM_PSIZE

    exp $_m_a_i_n
    pro $_m_a_i_n, 0

three
	rom 3
one
	rom 1

	/* Xor var with var */

	loe three
	loe one
	xor EM_WSIZE
	loc 2
	cmu EM_WSIZE
	zeq *1

    loc __LINE__
    cal $fail
    asp 4
1

	/* Xor var with const */

	loe three
	loc 1
	xor EM_WSIZE
	loc 2
	cmu EM_WSIZE
	zeq *2

    loc __LINE__
    cal $fail
    asp 4
2

	/* Xor const with var */

	loc 3
	loe one
	xor EM_WSIZE
	loc 2
	cmu EM_WSIZE
	zeq *3

    loc __LINE__
    cal $fail
    asp 4
3

	/* Xor var with big const */

	loe three
	loc 1001
	xor EM_WSIZE
	loc 1002
	cmu EM_WSIZE
	zeq *4

    loc __LINE__
    cal $fail
    asp 4
4

	/* Xor big const with var */

	loc 1001
	loe three
	xor EM_WSIZE
	loc 1002
	cmu EM_WSIZE
	zeq *5

    loc __LINE__
    cal $fail
    asp 4
5

    cal $finished
    end
