#
    mes 2, EM_WSIZE, EM_PSIZE

    exp $_m_a_i_n
    pro $_m_a_i_n, 0

zero
	rom 0
one
	rom 1

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

	/* Or var with const */

	loe zero
	loc 1
	ior EM_WSIZE
	loc 1
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

    cal $finished
    end
