#
    mes 2, EM_WSIZE, EM_PSIZE

    exp $_m_a_i_n
    pro $_m_a_i_n, 0

three
	rom 3
one
	rom 1
big
	rom 256

	/* And var with var */

	loe three
	loe one
	and EM_WSIZE
	loc 1
	cmu EM_WSIZE
	zeq *1

    loc __LINE__
    cal $fail
    asp 4
1

	/* And var with const */

	loe three
	loc 1
	and EM_WSIZE
	loc 1
	cmu EM_WSIZE
	zeq *2

    loc __LINE__
    cal $fail
    asp 4
2

	/* And const with var */

	loc 3
	loe one
	and EM_WSIZE
	loc 1
	cmu EM_WSIZE
	zeq *3

    loc __LINE__
    cal $fail
    asp 4
3

	/* And var with big const */

	loe big
	loc 257
	and EM_WSIZE
	loc 256
	cmu EM_WSIZE
	zeq *4

    loc __LINE__
    cal $fail
    asp 4
4

	/* And big const with var */

	loc 257
	loe big
	and EM_WSIZE
	loc 256
	cmu EM_WSIZE
	zeq *5

    loc __LINE__
    cal $fail
    asp 4
5

    cal $finished
    end
