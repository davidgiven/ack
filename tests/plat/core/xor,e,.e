#
    mes 2, EM_WSIZE, EM_PSIZE

    exp $_m_a_i_n
    pro $_m_a_i_n, 0

three
	rom 3
one
	rom 1
mone
	rom -1
big
	rom 1001

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

	/* Xor big const with negative var */

	loc 1001
	loe mone
	xor EM_WSIZE
	loc -1002
	cmu EM_WSIZE
	zeq *6

    loc __LINE__
    cal $fail
    asp 4
6

	/* Xor big var with negative const */

	loe big
	loc -1
	xor EM_WSIZE
	loc -1002
	cmu EM_WSIZE
	zeq *7

    loc __LINE__
    cal $fail
    asp 4
7

	/* Xor var with big low-byte-zero const */

	loe big
	loc 256
	xor EM_WSIZE
	loc 745
	cmu EM_WSIZE
	zeq *8

    loc __LINE__
    cal $fail
    asp 4
8


    cal $finished
    end
