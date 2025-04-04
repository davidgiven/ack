#
    mes 2, EM_WSIZE, EM_PSIZE

    exp $_m_a_i_n
    pro $_m_a_i_n, 0

zero
	rom 0
one
	rom 1
mone
	rom -1
big
	rom 258

	/* Or var with var */

	loe zero
	loe one
	ior EM_WSIZE
	loc 1
	cmu EM_WSIZE
	zeq *1

    loc __LINE__
    cal $fail
    asp 4
1

	/* Or big var with const */

	loe big
	loc 1
	ior EM_WSIZE
	loc 259
	cmu EM_WSIZE
	zeq *2

    loc __LINE__
    cal $fail
    asp 4
2

	/* Or const with var */

	loc 0
	loe one
	ior EM_WSIZE
	loc 1
	cmu EM_WSIZE
	zeq *3

    loc __LINE__
    cal $fail
    asp 4
3

	/* Or var with big const */

	loe zero
	loc 1000
	ior EM_WSIZE
	loc 1000
	cmu EM_WSIZE
	zeq *4

    loc __LINE__
    cal $fail
    asp 4
4

	/* Or big const with var */

	loc 1000
	loe one
	ior EM_WSIZE
	loc 1001
	cmu EM_WSIZE
	zeq *5

    loc __LINE__
    cal $fail
    asp 4
5

	/* Or big const with negative var */

	loc 1000
	loe mone
	ior EM_WSIZE
	loc -1
	cmu EM_WSIZE
	zeq *6

    loc __LINE__
    cal $fail
    asp 4
6

	/* Or big var with negative const */

	loe big
	loc -1
	ior EM_WSIZE
	loc -1
	cmu EM_WSIZE
	zeq *7

    loc __LINE__
    cal $fail
    asp 4
7

	/* Or var with big low-byte-zero const */

	loe big
	loc 256
	ior EM_WSIZE
	loc 258
	cmu EM_WSIZE
	zeq *8

    loc __LINE__
    cal $fail
    asp 4
8

    cal $finished
    end
