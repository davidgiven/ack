#
    mes 2, EM_WSIZE, EM_PSIZE

    exp $_m_a_i_n
    pro $_m_a_i_n, 0

	/* Or word-sized set */

four
	rom EM_WSIZE

	loc 0
	loc 1
	loe four		/* to defeat constant folding */
	ior
	loc 1
	cmu EM_WSIZE
	zeq *1

    loc __LINE__
    cal $fail
    asp 4
1

	/* Or triple-word-sized set */

four_by_three
	rom EM_WSIZE*3

	loc 16
	loc 32
	loc 64
	loc 1
	loc 2
	loc 3
	loe four_by_three
	ior
	loc 67
	cmu EM_WSIZE
	zne *2
	loc 34
	cmu EM_WSIZE
	zne *2
	loc 17
	cmu EM_WSIZE
	zeq *3
2
	loc __LINE__
	cal $fail
	asp 4
3

    cal $finished
    end
